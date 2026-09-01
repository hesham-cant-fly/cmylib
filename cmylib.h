/*
 * @file cmylib.h
 * Amalgamated header.
 * Generated automatically.
 */

#ifndef CMYLIB_H
#define CMYLIB_H

/* #define CMYLIB_IMPL */

#ifndef CMYLIB_DEF
#  define CMYLIB_DEF
#endif /* !CMYLIB_DEF */

#define CMYCOMMON_DEF CMYLIB_DEF
#define CMYCONTEXT_DEF CMYLIB_DEF
#define CMYSLICE_DEF CMYLIB_DEF
#define CMYTEST_DEF CMYLIB_DEF

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

CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file);
CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file);
CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file);

CMYCOMMON_DEF void m_panicf_(const char *file, int line, const char *fmt, ...);

/**
 * @file cmycontext.h
 * @author Hesham Can't Fly
 * @brief Context system similar to Odin's context
 * define CMYCONTEXT_DEF to static before including cmycontext.h if you want internal linkage.
 * # Example
 * @code
 * #define CMYCONTEXT_IMPL
 * #include "cmycontext.h"
 * #include <stdio.h>
 *
 * int main(void)
 * {
 *    init_context();
 *    context.id = 1;
 *    printf("%zu\n", context.id);
 *    PUSH_CONTEXT() {
 *        context.id += 1;
 *        printf("%zu\n", context.id);
 *    }
 * }
 * @endcode
 */
#include <stddef.h>
#include <stdlib.h>

#ifndef CMYCONTEXT_DEF
#  define CMYCONTEXT_DEF
#endif

/**
 * @brief access the current context
 */
#define context (m_context_stack_.items[m_context_stack_.len - 1])

/**
 * @brief push a new context and opens a new block. at the end it pops the context. it can be used like how you would use an if statement.
 * @warning doing `return` or a `goto` while inside this `PUSH_CONTEXT` block is not safe. as they don't allow for poping the context. if you want to do an early exit. use `continue;` or call `pop_context` before `return` or `goto`
 * @see push_context
 * @see pop_context
 */
#define PUSH_CONTEXT() for (int i_=(push_context(), 1); i_; i_=(pop_context(), 0))

/**
 * @brief a context type. feel free to modify it however you like.
 */
typedef struct context_t {
	size_t id;
} context_t;

typedef struct context_stack_t {
	size_t len, cap;
	context_t *items;
} context_stack_t;

extern context_t *m_current_context_;
extern context_stack_t m_context_stack_;

/**
 * @brief inittializes the context. call it only once at the top of `main`
 */
CMYCONTEXT_DEF void init_context(void);

/**
 * @brief pushes a new context
 */
CMYCONTEXT_DEF void push_context(void);

/**
 * @brief pops a context
 */
CMYCONTEXT_DEF void pop_context(void);

/**
 * @file cmyslice.h
 * @author Hesham Can't Fly
 * @brief Generic slice Library.
 * Everything here works for cmystring, cmyarray, cmystring_builder.
 * Generally. any struct that has a `len` and `*items` field will work with this library.
 *
 * # Example
 * @code
 * #include "cmyslice.h"
 * #include <stdio.h>
 *
 * typedef struct int_slice_t {
 *     SLICE_TYPE(int);
 * } int_slice_t;
 *
 * int main(void)
 * {
 *     int_slice_t ints = slice(int, 1, 2, 3);
 *     FORSLICE (int, item, ints) {
 *         printf("%d\n", item);
 *     }
 * }
 * @endcode
 */

#include <stddef.h>
#include "cmycommon.h"

/**
 * @brief Helper to define a slice type.
 * You just invoke this macro inside of the struct declaration itself and pass a type into it.
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     SLICE_TYPE(int);
 * } int_slice_t;
 * @endcode
 */
#define SLICE_TYPE(T_) \
	size_t len; \
	T_ *items

/**
 * @brief Creates a slice. like a slice literal.
 *
 * # Example
 * @code
 * int_slice_t ints = slice(int, 1, 2, 3);
 * function_that_takes_a_slice((int_slice_t)slice(int, 1, 2, 3)); // You have to "cast" it to your slice type.
 * @endcode
 */
#define slice(T_, ...) \
	{ .items = (T_[]){ __VA_ARGS__ }, .len = sizeof((T_[]){ __VA_ARGS__ })/sizeof(T_) }

/**
 * @brief Creates a slice. From pointer an a length.
 *
 * # Example
 * @code
 * int numbers[] = {1, 2, 3};
 * int_slice_t ints = slice_from_ptr(numbers, 3);
 * function_that_takes_a_slice((int_slice_t)slice_from_ptr(numbers, 3)); // You have to "cast" it to your slice type.
 * @endcode
 */
#define slice_from_ptr(ptr_, len_) \
	{ .items = (ptr_), .len = (len_), }

/**
 * @brief Access an item of slice_ with bounds check.
 * @warning THIS MAY CAUSE slice_ AND `...` TO BE EVALUATED MORE THAN ONCE.
 */
#define get(slice_, ...) (*(rget(slice_, __VA_ARGS__)))

/**
 * @brief Access an item of slice_ with bounds check. and returns a reference to it.
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define rget(slice_, ...) (((slice_).len <= (__VA_ARGS__)) ? (panicf("Accessed an index > len: %zu", (slice_).len), (slice_).items) : (&(((slice_).items)[__VA_ARGS__])))

/**
 * @brief Updates an item of slice_ at index_ with bounds check.
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define set(slice_, index_, ...) (*(rget(slice_, index_)) = (__VA_ARGS__))

/**
 * @brief Creates a sub-slice initializer `[begin_, end_)` from an existing slice.
 *
 * Performs runtime bounds checks using `panicf()`.
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 * @note Panics if `begin_ >= len` or `end_ >= len`.
 */
#define slicing(slice_, begin_, end_) \
	{ \
		.items = ((begin_ >= (slice_).len) ? (panicf("begin_ is > len: %zu", (slice_).len), NULL) : ((slice_).items + (begin_))), \
		.len = ((end_ >= (slice_).len) ? (panicf("end_ is > len: %zu", (slice_).len), 0) : ((end_) - (begin_))), \
	}

#define FORSLICE(type_, var_, slice_) \
	for (type_ *var_##_ptr_ = (slice_).items, \
	           *var_##_tmp_ = (void*)1, \
	           *var_##_end_ = (slice_).items + (slice_).len; \
		       (var_##_tmp_ == (void*)1) && var_##_ptr_ < var_##_end_; \
		       var_##_ptr_ += 1, var_##_tmp_ = (void*)(((char*)var_##_tmp_) + 1)) \
		for (type_ var_ = *var_##_ptr_; var_##_tmp_ == (void*)1; var_##_tmp_ = NULL)

#define IFORSLICE(type_, i_, var_, slice_) \
	for (type_ *var_##_start_ = (slice_).items, \
			   *var_##_ptr_ = (slice_).items, \
	           *var_##_tmp_ = (void*)1, \
	           *var_##_end_ = (slice_).items + (slice_).len; \
	           (var_##_tmp_ == (void*)1) && var_##_ptr_ < var_##_end_; \
	           var_##_ptr_ += 1, var_##_tmp_ = (void*)(((char*)var_##_tmp_) + 1)) \
		for (size_t i_=((var_##_ptr_) - (var_##_start_)), var_##_tmp2_ = 1; var_##_tmp2_; var_##_tmp2_=0) \
			for (type_ var_=*var_##_ptr_; var_##_tmp_ == (void*)1; var_##_tmp_ = NULL)

/**
 * @file cmytest.h
 * @author Hesham Can't Fly
 * @brief Super simple testing framework
 * @note If you want verbose output do `#define TEST_VERBOSE` before include cmytest.h
 * # Example
 * @code
 * #define TEST_VERBOSE // Enable verbose output
 * #include "cmytest.h"
 * // Everything you need to know is in this example
 * int add(int x, int y)
 * {
 *     return x + y
 * }
 *
 * TEST_CASE(add_test)
 * {
 *     TEST_ASSERT(add(1, 2) == 3);
 *     TEST_PASS();
 * }
 *
 * int main(void)
 * {
 *     TEST_SETUP();
 *     RUN_TEST(add_test);
 *     TEST_SUMMARY();
 * }
 * @endcode
 * @todo I wrote this header before I wrote CONVENTIONS.org .. so maybe I should update some stuff in here lol.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TEST_CLR_RESET   "\033[0m"
#define TEST_CLR_RED     "\033[31;1m"
#define TEST_CLR_GREEN   "\033[32;1m"
#define TEST_CLR_CYAN    "\033[36m"

#define TEST_PASS() return true
#define TEST_FAIL() return false

#define TEST_SETUP() \
	size_t total_tests = 0, failed_tests = 0

#ifdef TEST_VERBOSE
# define REPORT_TEST_ASSERT() \
	fprintf(stderr, "    [ %sASSERT%s    ] %s%40s%s -- (%s:%u)\n", TEST_CLR_CYAN, TEST_CLR_RESET, TEST_CLR_CYAN, __func__, TEST_CLR_RESET, __FILE__, __LINE__)
#else
# define REPORT_TEST_ASSERT()
#endif /* TEST_VERBOSE */
#define REPORT_TEST_ASSERT_FAILURE(...) \
	fprintf(stderr, "    [      %sFAIL%s ] %s%40s%s -- %s%s%s (%s:%u)\n", TEST_CLR_RED, TEST_CLR_RESET, TEST_CLR_CYAN, __func__, TEST_CLR_RESET, TEST_CLR_CYAN, #__VA_ARGS__, TEST_CLR_RESET, __FILE__, __LINE__)
#define REPORT_TEST_CASE_FAILURE(name_) \
	fprintf(stderr, "[      %sFAIL%s ] %s%44s%s -- (%s:%u)\n", TEST_CLR_RED, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)
#ifdef TEST_VERBOSE
# define REPORT_TEST_CASE(name_) \
	fprintf(stderr, "[ %sTEST%s      ] %s%44s%s -- (%s:%u)\n", TEST_CLR_GREEN, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)
# else
# define REPORT_TEST_CASE(name_)
#endif /* TEST_VERBOSE */
#define REPORT_TEST_CASE_OK(name_) \
	fprintf(stderr, "[ %sOK%s        ] %s%44s%s -- (%s:%u)\n", TEST_CLR_GREEN, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)

#define TEST_SUMMARY() \
	do { \
		fprintf(stderr, "\n[  %sSUMMARY%s  ] %s%zu Total%s -- %s%zu Failed%s\n", TEST_CLR_CYAN, TEST_CLR_RESET, TEST_CLR_GREEN, total_tests, TEST_CLR_RESET, TEST_CLR_RED, failed_tests, TEST_CLR_RESET); \
		return failed_tests == 0 ? 0 : 1; \
	} while (0)

#define TEST_CASE(name_) static bool name_(void)
#define RUN_TEST(name_) \
	do { \
		total_tests += 1; \
		REPORT_TEST_CASE(name_); \
		const bool result__ = name_(); \
		if (!result__) { \
			failed_tests += 1; \
			REPORT_TEST_CASE_FAILURE(name_); \
		} else { \
			REPORT_TEST_CASE_OK(name_); \
		} \
	} while (0)

#define TEST_ASSERT(...) \
	do { \
		REPORT_TEST_ASSERT(); \
		const bool result__ = (__VA_ARGS__); \
		if (!result__) { \
			REPORT_TEST_ASSERT_FAILURE(__VA_ARGS__); \
			TEST_FAIL(); \
		} \
	} while (0)


#ifdef CMYLIB_IMPL
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


CMYCONTEXT_DEF context_stack_t m_context_stack_ = {0};

CMYCONTEXT_DEF void init_context(void)
{
	push_context();
}

CMYCONTEXT_DEF void push_context(void)
{
	if (m_context_stack_.len >= m_context_stack_.cap) {
		m_context_stack_.cap += 20; // adding 20 because I think exponontial growth is not fitting here
		// FIXME: UB!! You can't pass a NULL to realloc
		m_context_stack_.items = realloc(m_context_stack_.items, m_context_stack_.cap);
	}

	m_context_stack_.len += 1;
	m_context_stack_.items[m_context_stack_.len - 1] = m_context_stack_.items[m_context_stack_.len - 2];
}

CMYCONTEXT_DEF void pop_context(void)
{
	if (m_context_stack_.len < 0) {
		return;
	}

	m_context_stack_.len -= 1;
}

#endif /* CMYLIB_IMPL */

#endif /* CMYLIB_H */
