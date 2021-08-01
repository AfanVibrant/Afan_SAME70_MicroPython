/****************************************************************************
 * interpreters/micropython/micropython_main.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Authors: Gregory Nutt <gnutt@nuttx.org>
 *            Dave Marples <dave@marples.net>
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
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <debug.h>
#include <stdio.h>
#include <math.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "lexer.h"
#include "parse.h"
#include "obj.h"
#include "compile.h"
#include "runtime0.h"
#include "runtime.h"

#include "py/compile.h"
#include "py/runtime.h"
#include "py/frozenmod.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mpconfig.h"
#include "py/stackctrl.h"
#include "nx_pyexec.h"
#include "nx_readline.h"

#ifdef CONFIG_ARCH_CHIP_SAMV71
#include "arm_arch.h"
#include "chip/sam_uid.h"
#include "chip/hardware/sam_rstc.h"
#endif

extern MP_NOINLINE bool execute_flash_fs(const char* file, bool show_fl);
extern MP_NOINLINE bool init_flash_fs(uint reset_mode);

#ifdef CONFIG_ARCH_CHIP_SAMV71
#define MICROPY_HW_BOARD_NAME "SAMV71-XULT"

static uint32_t get_reset_mode(void)
{
	uint32_t ret = 1;
	uint32_t regval = 0;

	regval = getreg32(SAM_RSTC_SR) & RSTC_SR_RSTTYP_MASK;
	if(regval & RSTC_SR_RSTTYP_PWRUP) {
		printf("%s power up\r\n",MICROPY_HW_BOARD_NAME);
	}
	else if(regval & RSTC_SR_RSTTYP_BACKUP) {
		printf("%s wake up from backup\r\n",MICROPY_HW_BOARD_NAME);
	}
	else if(regval & RSTC_SR_RSTTYP_WDOG) {
		printf("%s watchdog reset\r\n",MICROPY_HW_BOARD_NAME);
	}
	else if(regval & RSTC_SR_RSTTYP_SWRST) {
		printf("%s software reset\r\n",MICROPY_HW_BOARD_NAME);
	}
	else if(regval & RSTC_SR_RSTTYP_NRST) {
		printf("%s external reset\r\n",MICROPY_HW_BOARD_NAME);
	}

	return ret;
}

static void get_unique_id(void)
{
  uint8_t index, count;
  uint8_t uniqueid[16];

  sam_get_uniqueid(uniqueid);

  printf("Chip Unique ID = \r\n");
  printf("0x");
  for (index = 0, count = 0; index < 16; index++, count++)
    {
      if (count == 4)
        {
          printf(" 0x");
          count = 0;
        }
      printf("%02X", uniqueid[index]);
    }
  printf("\r\n");
}
#endif

#if MICROPY_ENABLE_COMPILER
void do_str(const char *src, mp_parse_input_kind_t input_kind) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, strlen(src), 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, true);
        mp_call_function_0(module_fun);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
}
#endif

#if MICROPY_ENABLE_GC
static char heap[CONFIG_INTERPRETERS_MICROPYTHON_HEAPSIZE];
#endif

int micropython_main(int argc, char **argv) {
// Thread should be initialized first
#if MICROPY_PY_THREAD
//mp_thread_init();
#endif

#ifdef CONFIG_ARCH_CHIP_SAMV71
    get_unique_id();
    get_reset_mode();
#endif

soft_reset:
	mp_stack_ctrl_init();
	// Make MicroPython's stack limit somewhat smaller than full stack available
	mp_stack_set_limit(CONFIG_INTERPRETERS_MICROPYTHON_STACKSIZE - 512);

#if MICROPY_ENABLE_GC
	gc_init(heap, heap + CONFIG_INTERPRETERS_MICROPYTHON_HEAPSIZE);
#endif

	mp_init();
	mp_obj_list_init(mp_sys_path, 0);
	mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_)); // current dir (or base dir of the script)
	mp_obj_list_init(mp_sys_argv, 0);

#if MICROPY_MODULE_FROZEN
	pyexec_frozen_module("main.py");
#endif

	for (;;) {
		if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) {
			if (pyexec_friendly_repl() != 0) {
				break;
			}
		}
		else if (pyexec_mode_kind == PYEXEC_MODE_RAW_REPL) {
			if (pyexec_raw_repl() != 0) {
				break;
			}
		}
	}

	printf("soft reboot\n");
	goto soft_reset;

	return 0;
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val) {
    while (1);
}

void NORETURN __fatal_error(const char *msg) {
    while (1);
}

#if !MICROPY_READER_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(ENOENT);
}
#endif

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
