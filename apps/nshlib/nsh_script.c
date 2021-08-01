/****************************************************************************
 * apps/nshlib/nsh_script.c
 *
 *   Copyright (C) 2007-2009, 2011-2013 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include "nsh.h"
#include "nsh_console.h"

#if CONFIG_NFILE_STREAMS > 0 && !defined(CONFIG_NSH_DISABLESCRIPT)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: nsh_script
 *
 * Description:
 *   Execute the NSH script at path.
 *
 ****************************************************************************/

int nsh_script(FAR struct nsh_vtbl_s *vtbl, FAR const char *cmd,
               FAR const char *path)
{
  FAR char *fullpath;
  FAR FILE *savestream;
  FAR char *buffer;
  FAR char *pret;
  int ret = ERROR;

  /* The path to the script may relative to the current working directory */

  fullpath = nsh_getfullpath(vtbl, path);
  if (!fullpath)
    {
      return ERROR;
    }

  /* Get a reference to the common input buffer */

  buffer = nsh_linebuffer(vtbl);
  if (buffer)
    {
      /* Save the parent stream in case of nested script processing */

      savestream = vtbl->np.np_stream;

      /* Open the file containing the script */

      vtbl->np.np_stream = fopen(fullpath, "r");
      if (!vtbl->np.np_stream)
        {
          nsh_error(vtbl, g_fmtcmdfailed, cmd, "fopen", NSH_ERRNO);

          /* Free the allocated path */

          nsh_freefullpath(fullpath);

          /* Restore the parent script stream */

          vtbl->np.np_stream = savestream;
          return ERROR;
        }

      /* Loop, processing each command line in the script file (or
       * until an error occurs)
       */

      do
        {
          /* Flush any output generated by the previous line */

          fflush(stdout);

#ifndef CONFIG_NSH_DISABLE_LOOPS
          /* Get the current file position.  This is used to control
           * looping.  If a loop begins in the next line, then this file
           * offset will be needed to locate the top of the loop in the
           * script file.  Note that ftell will return -1 on failure.
           */

          vtbl->np.np_foffs = ftell(vtbl->np.np_stream);
          vtbl->np.np_loffs = 0;

          if (vtbl->np.np_foffs < 0)
            {
              nsh_error(vtbl, g_fmtcmdfailed, "loop", "ftell", NSH_ERRNO);
            }
#endif

          /* Now read the next line from the script file */

          pret = fgets(buffer, CONFIG_NSH_LINELEN, vtbl->np.np_stream);
          if (pret)
            {
              /* Parse process the command.  NOTE:  this is recursive...
               * we got to cmd_source via a call to nsh_parse.  So some
               * considerable amount of stack may be used.
               */

              if ((vtbl->np.np_flags & NSH_PFLAG_SILENT) == 0)
                {
                  nsh_output(vtbl, "%s", buffer);
                }

              ret = nsh_parse(vtbl, buffer);
            }
        }
      while (pret && (ret == OK || (vtbl->np.np_flags & NSH_PFLAG_IGNORE)));

      /* Close the script file */

      fclose(vtbl->np.np_stream);

      /* Restore the parent script stream */

      vtbl->np.np_stream = savestream;
    }

  /* Free the allocated path */

  nsh_freefullpath(fullpath);
  return ret;
}

/****************************************************************************
 * Name: nsh_initscript
 *
 * Description:
 *   Attempt to execute the configured initialization script.  This script
 *   should be executed once when NSH starts.  nsh_initscript is idempotent
 *   and may, however, be called multiple times (the script will be executed
 *   once.
 *
 ****************************************************************************/

#ifdef CONFIG_NSH_ROMFSETC
int nsh_initscript(FAR struct nsh_vtbl_s *vtbl)
{
  static bool initialized;
  bool already;
  int ret = OK;

  /* Atomic test and set of the initialized flag */

  sched_lock();
  already     = initialized;
  initialized = true;
  sched_unlock();

  /* If we have not already executed the init script, then do so now */

  if (!already)
    {
      ret = nsh_script(vtbl, "init", NSH_INITPATH);

#ifndef CONFIG_NSH_DISABLESCRIPT
      /* Reset the option flags */

      vtbl->np.np_flags = NSH_NP_SET_OPTIONS_INIT;
#endif
    }

  return ret;
}

/****************************************************************************
 * Name: nsh_loginscript
 *
 * Description:
 *   Attempt to execute the configured login script.  This script
 *   should be executed when each NSH session starts.
 *
 ****************************************************************************/

#ifdef CONFIG_NSH_ROMFSRC
int nsh_loginscript(FAR struct nsh_vtbl_s *vtbl)
{
  return nsh_script(vtbl, "login", NSH_RCPATH);
}
#endif
#endif /* CONFIG_NSH_ROMFSETC */

#endif /* CONFIG_NFILE_STREAMS > 0 && !CONFIG_NSH_DISABLESCRIPT */
