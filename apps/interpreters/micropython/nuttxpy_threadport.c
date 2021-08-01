/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Kevin Liu for Vibrant Tech Ltd Co
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/init.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <stdio.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <mqueue.h>
#include <errno.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mpthread.h"
#include "py/gc.h"

#include "mpconfigport.h"
#include "mpthreadport.h"

#if MICROPY_PY_THREAD

#include <fcntl.h>
#include <signal.h>
#include <sched.h>

#include <nuttx/list.h>
#include <nuttx/nuttx.h>

#include "lib/utils/gchelper.h"

//#define MP_THREAD_MUTEX_DEBUG 1

#ifdef MP_THREAD_MUTEX_DEBUG
#define MP_THREAD_GIL_ENTERINFO()	printf("pid=%d locked\n",(int)getpid())
#define MP_THREAD_GIL_EXITINFO()	printf("pid=%d unlock\n",(int)getpid())
#else
#define MP_THREAD_GIL_ENTERINFO()
#define MP_THREAD_GIL_EXITINFO()
#endif

// this structure forms a linked list, one node per active thread
typedef struct _thread_t {
    mp_pthread_t id;        // system id of thread
    int ready;              // whether the thread is ready and running
    void *arg;              // thread Python args, a GC root pointer
    struct list_node list;
} thread_t;

STATIC pthread_key_t tls_key;

// the mutex controls access to the linked list
STATIC pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;
STATIC struct list_node g_thread_list;

// this is used to synchronize the signal handler of the thread
// it's needed because we can't use any pthread calls in a signal handler
STATIC sem_t thread_signal_done;

// this signal handler is used to scan the regs and stack of a thread
STATIC void mp_thread_gc(int signo, siginfo_t *info, void *context) {
    (void)info; // unused
    (void)context; // unused
    if (signo == SIGUSR1) {
        // get the registers and the sp
        uintptr_t regs[10];
        uintptr_t sp = gc_helper_get_regs_and_sp(regs);

        // trace the stack, including the registers (since they live on the stack in this function)
        gc_collect_root((void**)sp, ((uint32_t)MP_STATE_THREAD(stack_top) - sp) / sizeof(uint32_t));

        mp_thread_sem_post(&thread_signal_done);
    }
}

void mp_thread_init(void)
{
  pthread_key_create(&tls_key, NULL);
  pthread_setspecific(tls_key, &mp_state_ctx.thread);

  // create first entry in linked list of all threads
  list_initialize(&g_thread_list);

    // create first entry in linked list of all threads
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
  thread_t *thread = (thread_t *)malloc(sizeof(thread_t));
#pragma GCC diagnostic pop
  if (thread != NULL)
    {
      thread->id  = pthread_self();
      thread->arg = NULL;
      thread->ready = 1;
      list_initialize(&thread->list);
      list_add_tail(&thread->list, &g_thread_list);
    }

  mp_thread_sem_init(&thread_signal_done, 0);

  // enable signal handler for garbage collection
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = mp_thread_gc;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);
}

void mp_thread_deinit(void) {
  int ret;
  struct list_node* curr_list;

  pthread_mutex_lock(&thread_mutex);

  for (curr_list = g_thread_list.next; curr_list != &g_thread_list; ) {
    thread_t* thread_info = container_of(curr_list, thread_t, list);
    curr_list = thread_info->list.next;
    ret = pthread_cancel(thread_info->id);
    if (ret == OK)
      printf("pthread id=%d canceled\n", thread_info->id);
    list_delete(&thread_info->list);
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
    free(thread_info);
  #pragma GCC diagnostic pop
  }

  pthread_mutex_lock(&thread_mutex);
}

// This function scans all pointers that are external to the current thread.
// It does this by signaling all other threads and getting them to scan their
// own registers and stack.  Note that there may still be some edge cases left
// with race conditions and root-pointer scanning: a given thread may manipulate
// the global root pointers (in mp_state_ctx) while another thread is doing a
// garbage collection and tracing these pointers.

void mp_thread_gc_others(void) {
	struct list_node* curr_list;

    pthread_mutex_lock(&thread_mutex);
	for (curr_list = g_thread_list.next; curr_list != &g_thread_list; ) {
		thread_t* thread_info = container_of(curr_list, thread_t, list);
		curr_list = thread_info->list.next;

        if (thread_info->id == pthread_self()) {
            continue;
        }
        if (!thread_info->ready) {
            continue;
        }

        pthread_kill(thread_info->id, SIGUSR1);
        mp_thread_sem_pend(&thread_signal_done);
    }
    pthread_mutex_unlock(&thread_mutex);
}

mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t*)pthread_getspecific(tls_key);
}

void mp_thread_set_state(mp_state_thread_t* state) {
    pthread_setspecific(tls_key, state);
}

void mp_thread_start(void)
{
  struct list_node* curr_list;

  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  pthread_mutex_lock(&thread_mutex);
  for (curr_list = g_thread_list.next; curr_list != &g_thread_list; )
    {
      thread_t* thread_info = container_of(curr_list, thread_t, list);
      curr_list = thread_info->list.next;
      if (thread_info->id == pthread_self())
        {
          thread_info->ready = 1;
          break;
        }
    }
  pthread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void*), void *arg, size_t *stack_size) {
    mp_pthread_t id = -EINVAL;

    // default stack size is 8k machine-words
    if (*stack_size == 0) {
        *stack_size =  2048;  //8192 * BYTES_PER_WORD;
    }

    // minimum stack size is set by pthreads
    if (*stack_size < PTHREAD_STACK_MIN) {
        *stack_size = PTHREAD_STACK_MIN;
    }

    // set thread attributes
    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    if (ret != 0) {
        goto er;
    }
    ret = pthread_attr_setstacksize(&attr, *stack_size);
    if (ret != 0) {
        goto er;
    }

  pthread_mutex_lock(&thread_mutex);

  // create thread
  ret = pthread_create(&id, &attr, entry, arg);
  if (ret != 0)
    {
      pthread_mutex_unlock(&thread_mutex);
      goto er;
    }

  // adjust stack_size to provide room to recover from hitting the limit
  // this value seems to be about right for both 32-bit and 64-bit builds
  *stack_size -= 8192;

    // add thread to linked list of all threads
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
  thread_t *th = (thread_t *)malloc(sizeof(thread_t));
#pragma GCC diagnostic pop
  if (th != NULL)
    {
      th->id  = id;
      th->arg = arg;
      list_initialize(&th->list);
      list_add_tail(&th->list, &g_thread_list);
    }

  pthread_mutex_unlock(&thread_mutex);

  return;

er:
  mp_raise_OSError(ret);
}

int mp_thread_exit(mp_pthread_t thread_id) {
	int ret = -ENXIO;
	struct list_node* curr_list;

	pthread_mutex_lock(&thread_mutex);

	for (curr_list = g_thread_list.next; curr_list != &g_thread_list; ) {
		thread_t* thread_info = container_of(curr_list, thread_t, list);
		curr_list = thread_info->list.next;
		if (thread_info->id == thread_id) {
			void* res;
			ret = 0;
			pthread_cancel(thread_info->id);
			pthread_join(thread_info->id, &res);
			if (res == PTHREAD_CANCELED)
				printf("pthread id=%d canceled\n", thread_id);
			list_delete(&thread_info->list);
			gc_collect_root(&thread_info->arg, 1);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
			free(thread_info);
#pragma GCC diagnostic pop
		}
	}

	pthread_mutex_unlock(&thread_mutex);
	return ret;
}

void mp_thread_finish(void) {
	struct list_node* curr_list;

	pthread_mutex_lock(&thread_mutex);
	for (curr_list = g_thread_list.next; curr_list != &g_thread_list; )
    {
      thread_t* thread_info = container_of(curr_list, thread_t, list);
      curr_list = thread_info->list.next;
      if (thread_info->id == pthread_self())
        {
          list_delete(&thread_info->list);
          break;
        }
    }
  pthread_mutex_unlock(&thread_mutex);
}

#if MICROPY_PY_THREAD_GIL

bool mp_thread_check_and_unlock(mp_thread_mutex_t *mutex) {
	bool ret = false;
	if (mutex->pid == (int)getpid()) {
		pthread_mutex_lock(&thread_mutex);
		struct list_node* curr_list;
		for (curr_list = g_thread_list.next; curr_list != &g_thread_list; ) {
			thread_t* thread_info = container_of(curr_list, thread_t, list);
			curr_list = thread_info->list.next;
			if (thread_info->id == mutex->pid) {
				ret = true;
				mp_thread_mutex_unlock(mutex);
				MP_THREAD_GIL_EXITINFO();
				break;
			}
		}

		pthread_mutex_unlock(&thread_mutex);
	}

	return ret;
}

void mp_thread_check_and_lock(mp_thread_mutex_t *mutex, bool val)
{
  if (val)
    {
      mp_thread_mutex_lock(mutex, 1);
      MP_THREAD_GIL_ENTERINFO();
    }
}

void mp_thread_mutex_init(mp_thread_mutex_t *mutex)
{
  if (mutex != NULL)
    {
      pthread_mutex_init(mutex, NULL);
    }
}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {
    int ret;
    if (wait) {
        ret = pthread_mutex_lock(mutex);
        if (ret == 0) {
            return 1;
        }
    } else {
        ret = pthread_mutex_trylock(mutex);
        if (ret == 0) {
            return 1;
        } else if (ret == EBUSY) {
            return 0;
        }
    }
    return -ret;
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    pthread_mutex_unlock(mutex);
    // TODO check return value
}

#endif // MICROPY_PY_THREAD_GIL

#endif // MICROPY_PY_THREAD
