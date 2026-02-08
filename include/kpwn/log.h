/**
 * kpwn/log.h — Logging, assertions and debug macros
 *
 * Included by every other kpwn header.
 *
 * Provides:
 *   INFO(fmt, ...)      blue  [*]  informational
 *   OK(fmt, ...)        green [+]  success
 *   ERR(fmt, ...)       red   [-]  error (non-fatal)
 *   WARN(fmt, ...)      yellow[!]  warning
 *   DIE(fmt, ...)       red   [!!] fatal — prints and calls exit(1)
 *   LEAK(sym)           blue  [Addr] print name + value of a pointer
 *   ASSERT(cond)        abort if cond is false
 *
 * All output goes to stderr (fd 2) via dprintf
 */
#ifndef KPWN_LOG_H
#define KPWN_LOG_H

#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Colors (ANSI) */
#define _CLR_RESET   "\033[0m"
#define _CLR_BLUE    "\033[34m"
#define _CLR_GREEN   "\033[32m"
#define _CLR_RED     "\033[31m"
#define _CLR_YELLOW  "\033[33m"

/* Core log macros */
#define INFO(fmt, ...) \
    dprintf(2, _CLR_BLUE  "[*] " fmt _CLR_RESET "\n", ##__VA_ARGS__)

#define OK(fmt, ...) \
    dprintf(2, _CLR_GREEN "[+] " fmt _CLR_RESET "\n", ##__VA_ARGS__)

#define ERR(fmt, ...) \
    dprintf(2, _CLR_RED   "[-] " fmt _CLR_RESET "\n", ##__VA_ARGS__)

#define WARN(fmt, ...) \
    dprintf(2, _CLR_YELLOW "[!] " fmt _CLR_RESET "\n", ##__VA_ARGS__)

#define DIE(fmt, ...) do {                                          \
    dprintf(2, _CLR_RED "[!!] " fmt _CLR_RESET "\n", ##__VA_ARGS__); \
    exit(EXIT_FAILURE);                                              \
} while (0)

/* Leak helper */
#define LEAK(sym) \
    dprintf(2, _CLR_BLUE "[Addr] %-24s %p" _CLR_RESET "\n", #sym, (void *)(u64)(sym))

/* Assert */
#define ASSERT(cond) do {                                            \
    if (__builtin_expect(!(cond), 0)) {                              \
        DIE("Assertion failed: %s  (%s:%d)", #cond, __FILE__, __LINE__); \
    }                                                                \
} while (0)

/* Stringify helpers */
#define __xstr(s)  #s
#define XSTR(s)    __xstr(s)

#endif /* KPWN_LOG_H */
