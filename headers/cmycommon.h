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

#ifndef CMYCOMMON_H
#define CMYCOMMON_H

/* #define CMYCOMMON_IMPL */



#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

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

#ifndef alignof
/**
 * @brief Portably get the alignment of a type T_.
 */
#  define alignof(T_) (offsetof(struct { char x; T_ target; }, target))
#endif

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

/**
 * @brief Mark a symbol as unused to suppress the unused variable warning.
 */
#define unused(...) (void)(__VA_ARGS__)

/**
 * @brief Set every byte in ptr_ to 0
 */
#define setzero(ptr_) memset((ptr_), 0, sizeof(*(ptr_)))

CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file);
CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file);
CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file);

CMYCOMMON_DEF void m_panicf_(const char *file, int line, const char *fmt, ...);


#ifdef CMYCOMMON_IMPL
CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file)
{
#ifdef CMYCOMMON_USE_CALLOC
	void *result = calloc(1, size);
#else
	void *result = malloc(size);
#endif
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling malloc(%zu).\n", file, line, size);
		exit(1);
	}

	return result;
}

CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file)
{
	if (ptr == NULL) {
		return m_xmalloc_(new_size, line, file);
	}

	void *result = realloc(ptr, new_size);
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling realloc.\n", file, line);
		exit(1);
	}

	return result;
}

CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file)
{
	void *result = calloc(number, size);
	if (result == NULL) {
		fprintf(stderr, "%s:%d: Not Enough Memory when calling calloc(%zu, %zu).\n", file, line, number, size);
		exit(1);
	}

	return result;
}

CMYCOMMON_DEF void m_panicf_(const char *file, int line, const char *fmt, ...)
{
	fprintf(stderr, "%s:%d: ", file, line);
	
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(1);
}

#endif /* CMYCOMMON_IMPL */

#endif /* CMYCOMMON_H */
