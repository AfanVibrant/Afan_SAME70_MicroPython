/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Author: Kevin Liu    <lj2057@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <nuttx/config.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "extmod/misc.h"

#ifndef __APPS_EXAMPLES_MP_MPHALPORT_NUTTX_H
#define __APPS_EXAMPLES_MP_MPHALPORT_NUTTX_H

// Global millisecond tick count (driven by SysTick interrupt).
extern volatile uint64_t ticks_ms;

static inline mp_uint_t mp_hal_ticks_ms(void) {
#ifdef CONFIG_CLOCK_MONOTONIC
    struct timespec tv;
    MP_THREAD_GIL_EXIT();
    clock_gettime(CLOCK_MONOTONIC, &tv);
    MP_THREAD_GIL_ENTER();
    return tv.tv_sec * 1000 + tv.tv_nsec / 1000000;
#else
    struct timeval tv;
    MP_THREAD_GIL_EXIT();
    gettimeofday(&tv, NULL);
    MP_THREAD_GIL_ENTER();
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

static inline mp_uint_t mp_hal_ticks_us(void) {
#ifdef CONFIG_CLOCK_MONOTONIC
    struct timespec tv;
    MP_THREAD_GIL_EXIT();
    clock_gettime(CLOCK_MONOTONIC, &tv);
    MP_THREAD_GIL_ENTER();
    return tv.tv_sec * 1000000 + tv.tv_nsec / 1000;
#else
    struct timeval tv;
    MP_THREAD_GIL_EXIT();
    gettimeofday(&tv, NULL);
    MP_THREAD_GIL_ENTER();
    return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}

static inline mp_uint_t mp_hal_ticks_cpu(void) {
    // ticks_cpu() is defined as using the highest-resolution timing source
    // in the system. This is usually a CPU clock, but doesn't have to be,
    // here we just use NuttX tick timer.
    return (mp_uint_t)ticks_ms;
}

// Core delay function that does an efficient sleep and may switch thread context.
// If IRQs are enabled then we must have the GIL.
static inline void mp_hal_delay_ms(mp_uint_t Delay) {
	sem_t sem_wait;
	uint32_t ticks = (Delay / MSEC_PER_TICK);
	nxsem_init(&sem_wait, 0, 0);
	MP_THREAD_GIL_EXIT();
	nxsem_tickwait(&sem_wait, clock_systimer(), ticks);
	MP_THREAD_GIL_ENTER();
}

// delay for given number of microseconds
static inline void mp_hal_delay_us(mp_uint_t usec) {
	MP_THREAD_GIL_EXIT();
	usleep(usec);
	MP_THREAD_GIL_ENTER();
}

#endif // __APPS_EXAMPLES_MP_MPHALPORT_NUTTX_H
