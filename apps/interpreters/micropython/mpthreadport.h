/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#ifndef __APPS_EXAMPLES_MP_MPTHREADPORT_NUTTX_H
#define __APPS_EXAMPLES_MP_MPTHREADPORT_NUTTX_H

#include <nuttx/config.h>

#include <pthread.h>

#include <nuttx/semaphore.h>

#if 1

typedef sem_t            mp_thread_sem_t;
typedef pthread_t        mp_pthread_t;
typedef pthread_mutex_t  mp_thread_mutex_t;
typedef pthread_rwlock_t mp_thread_rwlock_t;
typedef pthread_attr_t   mp_thread_attr_t;

/* MicroPython Semaphore related API */

#define mp_thread_sem_init(x, y)           nxsem_init(x, 0, y)
#define mp_thread_sem_pend(x)              nxsem_wait(x)
#define mp_thread_sem_post(x)              nxsem_post(x)
#define mp_thread_sem_setval(x, y)         nxsem_reset(x, y)
#define mp_thread_sem_getval(x, y)         nxsem_getvalue(x, y)
#define mp_thread_sem_delete(x)            nxsem_destroy(x)
#define mp_thread_sem_trypend(x)           nxsem_trywait(x)
#define mp_thread_sem_timedwait(x, y)      nxsem_timedwait(x, y)
#define mp_thread_sem_tickwait(x, y)       nxsem_tickwait(x, clock_systimer(), y)
#define mp_thread_sem_setprotocol(x, y)    nxsem_setprotocol(x, y)

/* MicroPython Mutex Related API */

//#define mp_thread_mutex_init(x)            pthread_mutex_init(x, NULL)
//#define mp_thread_mutex_lock(x)            pthread_mutex_lock(x)
//#define mp_thread_mutex_unlock(x)          pthread_mutex_unlock(x)
#define mp_thread_mutex_timedlock(x, y)    pthread_mutex_timedlock(x, y)
#define mp_thread_mutex_trylock(x)         pthread_mutex_trylock(x)
#define mp_thread_mutex_delete(x)          pthread_mutex_destroy(x)

/* MicroPython RWLock Related API */

#define mp_thread_rwlock_init(x)           pthread_rwlock_init(x, NULL)
#define mp_thread_rwlock_rdlock(x)         pthread_rwlock_rdlock(x)
#define mp_thread_rwlock_wrlock(x)         pthread_rwlock_wrlock(x)
#define mp_thread_rwlock_unlock(x)         pthread_rwlock_unlock(x)
#define mp_thread_rwlock_timedrdlock(x, y) pthread_rwlock_timedrdlock(x, y)
#define mp_thread_rwlock_timedwrlock(x, y) pthread_rwlock_timedwrlock(x, y)
#define mp_thread_rwlock_tryrdlock(x)      pthread_rwlock_tryrdlock(x)
#define mp_thread_rwlock_trywrlock(x)      pthread_rwlock_trywrlock(x)
#define mp_thread_rwlock_delete(x)         pthread_rwlock_destroy(x)

/* MicroPython Pthread Related API */

#define mp_thread_attr_init(x)               pthread_attr_init(x)
#define mp_thread_attr_destroy(x)            pthread_attr_destro(x)

#define mp_thread_attr_setschedpolicy(x, y)  pthread_attr_setschedpolicy(x, y)
#define mp_thread_attr_getschedpolicy(x, y)  pthread_attr_setschedpolicy(x, y)

#define mp_thread_attr_setschedparam(x, y)   pthread_attr_setschedparam(x, y)
#define mp_thread_attr_getschedparam(x, y)   pthread_attr_getschedparam(x, y)

#define mp_thread_attr_setinheritsched(x, y) pthread_attr_setinheritsched(x, y)
#define mp_thread_attr_getinheritsched(x, y) pthread_attr_getinheritsched(x, y)

#define mp_thread_attr_setstacksize(x, y)    pthread_attr_setstacksize(x, y)
#define mp_thread_attr_getstacksize(x, y)    pthread_attr_getstacksize(x, y)
#define mp_thread_attr_setstack(x, y, z)     pthread_attr_setstack(x, y, z)
#define mp_thread_attr_getstack(x, y, z)     pthread_attr_getstack(x, y, z)

//#define mp_thread_create(x, y, z)            pthread_create(x, y, z)
#define mp_thread_detach(x)                  pthread_detach(x)
//#define mp_thread_exit(x)                    pthread_exit(x)
#define mp_thread_cancel(x)                  pthread_cancel(x)
#define mp_thread_setcancelstate(x, y)       pthread_setcancelstate(x, y)
#define mp_thread_setcanceltype(x, y)        pthread_setcanceltype(x, y)
#define mp_thread_join(x, y)                 pthread_join(x, y)
#define mp_thread_yield()                    pthread_yield()
#define mp_thread_kill(x, y)                 pthread_kill(x, y)

#define mp_thread_getschedparam(x, y, z)     pthread_getschedparam(x, y, z)
#define mp_thread_setschedparam(x, y, z)     pthread_setschedparam(x, y, z)

#define mp_thread_setschedprio(x, y)         pthread_setschedprio(x, y)

#define mp_thread_key_create(x, y)           pthread_key_create(x, y)
#define mp_thread_key_delete(x)              pthread_key_delete(x)

#define mp_thread_setspecific(x, y)          pthread_setspecific(x, y)
#define mp_thread_getspecific(x)             pthread_getspecific(x)

#define mp_thread_condattr_init(x)           pthread_condattr_init(x)
#define mp_thread_condattr_destroy(x)        pthread_condattr_destroy(x)

#define mp_thread_cond_init(x, y)            pthread_cond_init(x, y)
#define mp_thread_cond_destroy(x)            pthread_cond_destroy(x)
#define mp_thread_cond_signal(x)             pthread_cond_signal(x)
#define mp_thread_cond_wait(x, y)            pthread_cond_wait(x, y)
#define mp_thread_cond_timedwait(x, y, z)    pthread_cond_timedwait(x, y, z)

#define mp_thread_sigmask(x, y, z)           pthread_sigmask(x, y, z)

int mp_thread_exit(mp_pthread_t thread_id);

#else

#endif

void mp_thread_init(void);
void mp_thread_deinit(void);
void mp_thread_gc_others(void);

#endif // __APPS_EXAMPLES_MP_MPTHREADPORT_NUTTX_H
