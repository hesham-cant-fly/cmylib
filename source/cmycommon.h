/**
 * @file cmycommon.h
 * @author Hesham Can't Fly
 * @brief Common functions I might use
 * # OPTIONS
 * @code
 * #define CMYCOMMON_DEF static
 * #define CMYCOMMON_USE_CALLOC // xmalloc will use calloc under the hood
 * #define CMYCOMMON_IMPL
 * #include "cmycommon.h"
 * @endcode
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef CMYCOMMON_DEF
#  define CMYCOMMON_DEF
#endif /* !CMYCOMMON_DEF */

#ifndef NULL
#  define NULL ((void*)0x0)
#endif /* !NULL */

/**
 * @brief a wrapper on top of malloc that panics when `malloc` returns NULL
 */
#define xmalloc(size_) m_xmalloc_((size_), __LINE__, __FILE__)

/**
 * @brief a wrapper on top of realloc that uses `malloc` if `ptr` is NULL. otherwise it uses `realloc` and panics if NULL is returned.
 */
#define xrealloc(ptr_, new_size_) m_xrealloc_((ptr_), (new_size_), __LINE__, __FILE__)

/**
 * @brief a wrapper on top of calloc. panics if it returns NULL.
 */
#define xcalloc(number_, size_) m_xcalloc_((number_), (size_), __LINE__, __FILE__)

/**
 * @brief fprintf fmt_ with format to stderr and exits with code of 1
 */
#define panicf(...) (m_panicf_(__FILE__, __LINE__, __VA_ARGS__))

/**
 * @brief Execlusive iteration from start_ to `...`
 *
 * @param n_ The counter's name which is always gonna be of type size_t
 * @param start_ Where the iteration begins. It must be an integer value.
 * @param ... Where the iteration ends. It must be an integer value.
 *
 * # Example
 * @code
 * FORANGE(i, 0, 10) {
 *     printf("%zu\n", i);
 * }
 * @endcode
 */
#define FORANGE(n_, start_, ...) \
	for (size_t n_ = (start_); (n_) < (__VA_ARGS__); (n_) += 1)

CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file);
CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file);
CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file);

CMYCOMMON_DEF void m_panicf_(const char *file, int line, const char *fmt, ...);
