typedef int wint_t;

#include <stdint.h>

// options to control how MicroPython is built

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
//#define MICROPY_ENABLE_COMPILER          (0)

#define MICROPY_QSTR_BYTES_IN_HASH       (2)
//#define MICROPY_QSTR_EXTRA_POOL          mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX           (512)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT   (16)
#define MICROPY_EMIT_X64                 (0)
#define MICROPY_EMIT_THUMB               (0)
#define MICROPY_EMIT_INLINE_THUMB        (0)
#define MICROPY_EMIT_INLINE_THUMB_ARMV7M (0)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT  (0)
#define MICROPY_COMP_MODULE_CONST        (0)
#define MICROPY_COMP_CONST               (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_MEM_STATS                (0)
#define MICROPY_DEBUG_PRINTERS           (0)
#define MICROPY_ENABLE_GC                (1)
#define MICROPY_GC_ALLOC_THRESHOLD       (1)
#define MICROPY_REPL_EVENT_DRIVEN        (0)
#define MICROPY_HELPER_REPL              (1)
#define MICROPY_REPL_AUTO_INDENT         (1)
#define MICROPY_HELPER_LEXER_UNIX        (0)
#define MICROPY_ENABLE_SOURCE_LINE       (0)
#define MICROPY_ENABLE_DOC_STRING        (0)
#define MICROPY_ERROR_REPORTING          (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_MACHINE               (1)
#define MICROPY_PY_MACHINE_PULSE	     (1)
#define MICROPY_PY_ASYNC_AWAIT           (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO  (1)

#define MICROPY_PY_BUILTINS_BYTEARRAY    (1)
#define MICROPY_PY_BUILTINS_ENUMERATE    (1)
#define MICROPY_PY_BUILTINS_FILTER       (1)
#define MICROPY_PY_BUILTINS_HELP         (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_BUILTINS_INPUT        (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW   (1)
#define MICROPY_PY_BUILTINS_MIN_MAX      (1)
#define MICROPY_PY_BUILTINS_PROPERTY     (1)
#define MICROPY_PY_BUILTINS_REVERSED     (1)
#define MICROPY_PY_BUILTINS_SET          (1)
#define MICROPY_PY_BUILTINS_SLICE        (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS  (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE  (1)

#define MICROPY_NONSTANDARD_TYPECODES    (0)
#define MICROPY_PY___FILE__              (1)
#define MICROPY_PY_THREAD                (1)
#define MICROPY_PY_THREAD_GIL            (1)
#define MICROPY_PY_GC                    (1)
#define MICROPY_PY_ARRAY                 (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN    (1)
#define MICROPY_PY_ATTRTUPLE             (0)
#define MICROPY_PY_COLLECTIONS           (0)
#define MICROPY_PY_MATH                  (1)
#define MICROPY_PY_CMATH                 (1)
#define MICROPY_PY_IO                    (0)
#define MICROPY_PY_STRUCT                (0)
#define MICROPY_PY_SYS                   (1)
#define MICROPY_PY_SYS_MAXSIZE           (1)
#define MICROPY_PY_SYS_MODULES           (0)
#define MICROPY_PY_SYS_EXIT              (0)
#define MICROPY_PY_SYS_STDFILES          (0)
#define MICROPY_PY_SYS_STDIO_BUFFER      (0)
#define MICROPY_PY_UTIME_MP_HAL          (1)
#define MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM (0)
#define MICROPY_MODULE_FROZEN_MPY        (1)
#define MICROPY_CPYTHON_COMPAT           (0)
#define MICROPY_LONGINT_IMPL             (MICROPY_LONGINT_IMPL_NONE)
#define MICROPY_ENABLE_SCHEDULER         (1)
#define MICROPY_SCHEDULER_DEPTH          (8)
#define MICROPY_NLR_X86                  (0)
#define MICROPY_NLR_X64                  (0)
#define MICROPY_NLR_POWERPC              (0)
#define MICROPY_NLR_XTENSA               (0)
#define MICROPY_NLR_SETJMP               (0)
#define MICROPY_PY_OS_DUPTERM            (1)
#define MICROPY_PY_LWIP                  (0)
#define MICROPY_PY_WEBREPL               (0)
#define MICROPY_PY_BLUETOOTH             (0)
#define MICROPY_VFS                      (0)
#define MICROPY_VFS_FAT                  (MICROPY_VFS)
#define MICROPY_READER_VFS               (MICROPY_VFS)
#define MICROPY_FSUSERMOUNT              (0)
#define MICROPY_PY_LWIP_SLIP             (0)
#define N_ARM                            (0)
#define N_X64                            (0)
#define N_X86                            (0)
#define N_THUMB                          (0)
#define N_XTENSA                         (0)
#define USE_RULE_NAME                    (0)
#define MP_NEED_LOG2                     (0)
#define BYTES_PER_WORD                   (4)
#define GENERIC_ASM_API                  (0)

#define MICROPY_VFS_POSIX                (0)
#define MICROPY_STACK_CHECK              (1)
#define MICROPY_KBD_EXCEPTION            (1)
#define MICROPY_PERSISTENT_CODE_LOAD     (1)

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// This is an inclusive list that should correspond to the NX_MICROPYTHON_xx list above,
// including dependencies such as NX_MACHINE depending on NX BOARD (shown by indentation).
// Some of these definitions will be blank depending on what is turned on and off.
// Some are omitted because they're in MICROPY_PORT_BUILTIN_MODULE_BOARD_LINKS above.
#define MICROPY_PORT_BUILTIN_MODULES_NX_LINKS

#define MICROPY_PORT_BUILTIN_MODULE_BOARD_LINKS \

#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_PORT_BUILTIN_MODULES_NX_LINKS \
    MICROPY_PORT_BUILTIN_MODULE_BOARD_LINKS

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#ifdef __linux__
#define MICROPY_MIN_USE_STDOUT (1)
#endif

#define MP_STATE_PORT MP_STATE_VM

#include "supervisor/flash_root_pointers.h"

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    mp_obj_t terminal_tilegrid_tiles; \
    FLASH_ROOT_POINTERS \

/*    mp_obj_t machine_pin_irq_handler[ARCH_CHIP_PIN_MAX]; */

//#define mp_hal_stdout_tx_str(str)	fprintf(stdout, str)

#include "pthread.h"

typedef pthread_mutex_t critical_mutex_t;
typedef sem_t critical_sem_t;

#if MICROPY_PY_THREAD

#define critical_mutex_init(x)     pthread_mutex_init(x, NULL)
#define critical_mutex_lock(x)     pthread_mutex_lock(x)
#define critical_mutex_unlock(x)   pthread_mutex_unlock(x)

#define critical_sem_init(x,y,z)        nxsem_init(x,y,z)
#define critical_sem_delete(x)          nxsem_destroy(x)
#define critical_sem_wait(x)            nxsem_wait(x)
#define critical_sem_waittimeout(x,y,z) nxsem_tickwait(x,y,z)
#define critical_sem_send(x)            nxsem_post(x)
#define critical_sem_settype(x,y)       nxsem_setprotocol(x,y)

#else

#define critical_mutex_init(x)
#define critical_mutex_lock(x)
#define critical_mutex_unlock(x)

#define critical_sem_init(x,y,z)
#define critical_sem_delete(x)
#define critical_sem_wait(x)
#define critical_sem_waittimeout(x,y,z)
#define critical_sem_send(x)
#define critical_sem_settype(x,y)

#endif
