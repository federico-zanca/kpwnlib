/**
 * kpwn/utils.h — Common utility functions
 *
 * Helpers that almost every exploit needs:
 *   - open_dev()     open a device node or die
 *   - hexdump()      pretty-print a memory region
 *   - pin_cpu()      pin the current process to CPU
 *   - pinCPU()        pin to CPU 0 (convenience wrapper around pin_cpu)
 *   - get_shell()    execve /bin/sh (post-privesc)
 *   - save_state()   snapshot user CS/SS/RSP/RFLAGS
 *   - restore_state() return to user-space with iretq
 *   - debug helpers  utils_init(), DEBUG(), breakpoints
 *
 * Corresponding implementation: src/utils.c
 */
#ifndef KPWN_UTILS_H
#define KPWN_UTILS_H

#include "types.h"
#include "log.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>

/* Device interaction */

/**
 * open_dev - Open a device node, abort on failure.
 * @path:  e.g. "/dev/vulndev"
 * @flags: e.g. O_RDWR
 */
int open_dev(const char *path, int flags);

/* Memory helpers */

/**
 * hexdump - Print a hex dump of @len bytes at @addr.
 * @addr:  start of data
 * @len:   number of bytes
 */
void hexdump(const void *addr, size_t len);

/* CPU affinity */

/**
 * pin_cpu - Pin the calling thread to CPU @cpu.
 *           Returns 0 on success, -1 on error.
 */
int pin_cpu(int cpu);
#define pinCPU() pin_cpu(0)

/* Post-privesc */

/**
 * get_shell - execve("/bin/sh") — call after becoming root.
 */
void get_shell(void);

/* Debug helpers */

/**
 * utils_init - Parse argv for "DEBUG" and enable debug if present.
 *              Call this at the start of main().
 */
void utils_init(int argc, char **argv);

void utils_set_debug(int enabled);

/* Exposed so DEBUG() can be header-only without a function call. */
extern int g_kpwn_debug;

#define utils_debug_enabled()  (g_kpwn_debug)

/**
 * utils_debug_pause - Wait for Enter on stdin (blocks).
 *                     Used internally by DEBUG() when debug is on.
 */
void utils_debug_pause(void);

/**
 * DEBUG - Conditional debug logging with file:line + optional pause.
 *         When debug is disabled, falls back to INFO().
 */
#define DEBUG(fmt, ...) do {                                              \
    if (utils_debug_enabled()) {                                          \
        dprintf(2, "\n" _CLR_YELLOW "### %s:%d " fmt                     \
                _CLR_RESET "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
        utils_debug_pause();                                              \
    } else {                                                              \
        INFO(fmt, ##__VA_ARGS__);                                         \
    }                                                                     \
} while (0)

/**
 * BREAKPOINT - Emit INT3. Attach gdb to hit it.
 */
#define BREAKPOINT()  __asm__ volatile("int3")

/* User-space context (used for iretq / trampoline returns) */
typedef struct {
	u64 cs;
	u64 ss;
	u64 rsp;
	u64 rflags;
} user_state_t;

/**
 * save_state - Snapshot CS, SS, RSP, RFLAGS into @state.
 *              Call this early in main(), before triggering the vuln.
 */
void save_state(user_state_t *state);

/**
 * restore_state - Return to user-space using iretq and @state.
 */
void restore_state(user_state_t *state);

/* pwnhub-style debug stubs (kept here to avoid extra files) */
void test_user_space_asm(void);
void test_user_space_func(void);

/* Pointer & alignment macros */
#define PTR_ADD(ptr, off)       ((void *)((u8 *)(ptr) + (off)))
#define PTR_SUB(ptr, off)       ((void *)((u8 *)(ptr) - (off)))
#define ALIGN_UP(x, a)          (((x) + ((a)-1)) & ~((a)-1))
#define ALIGN_DOWN(x, a)        ((x) & ~((a)-1))
#define IS_ALIGNED(ptr, align)  (((uintptr_t)(ptr) & ((align)-1)) == 0)

#endif /* KPWN_UTILS_H */
