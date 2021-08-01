/****************************************************************************
 * interpreters/micropython/samv71_flash.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Authors: Kevin Liu (lj2057@163.com)
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <debug.h>
#include <stdio.h>
#include <math.h>

#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"

#include "nx_pyexec.h"
#include "mpconfigport.h"

static inline bool check_flash_fs(const char* file, bool show_fl) {
	bool ret = false;
	struct stat file_stat;
	
	if(stat(file, &file_stat) >= 0){
		if(show_fl) {
			byte* fbuf = (byte* )malloc(file_stat.st_size);
			if (fbuf != NULL) {
				FILE * fp = fopen(file,"r");
				if (fp != NULL) {
					memset(fbuf, 0, file_stat.st_size);
					if (fread(fbuf, file_stat.st_size, 1, fp) > 0) {
						mp_hal_stdout_tx_str("begin to execute ");
						mp_hal_stdout_tx_str(file);
						mp_hal_stdout_tx_str("\r\n");
						mp_hal_stdout_tx_str("/**********************************\r\n");
						mp_hal_stdout_tx_str((const char*)fbuf);
						mp_hal_stdout_tx_str("/**********************************\r\n");
						mp_hal_stdout_tx_str(file);			
						mp_hal_stdout_tx_str(" executed output\r\n");
					}
					fclose(fp);
				}
				m_free(fbuf);
			}	
		}
		ret = true;
	}

	return ret;
}

// avoid inlining to avoid stack usage within main()
MP_NOINLINE bool execute_flash_fs(const char* file, bool show_fl) {
	bool ret = true;
	
	if(check_flash_fs(file, show_fl) == true){		
		int result = pyexec_file(file);
		if (result & PYEXEC_FORCED_EXIT) {
			ret = false;
			mp_hal_stdout_tx_str("failed to execute file\n");
		}
	}	
	
	return ret;
}
