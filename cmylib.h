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
#define CMYALLOCATOR_DEF CMYLIB_DEF
#define CMY_C_ALLOCATOR_DEF CMYLIB_DEF
#define CMY_DEBUG_ALLOCATOR_DEF CMYLIB_DEF
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
#include <stddef.h>
#include <stdalign.h>

#ifndef CMYCOMMON_DEF
#  define CMYCOMMON_DEF
#endif /* !CMYCOMMON_DEF */

#ifndef NULL
#  define NULL ((void*)0x0)
#endif /* !NULL */

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199409L
#  define C_STD_90 1
#else
#  define C_STD_90 0
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  define C_STD_99 1
#else
#  define C_STD_99 0
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define C_STD_11 1
#else
#  define C_STD_11 0
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201710L
#  define C_STD_17 1
#else
#  define C_STD_17 0
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#  define C_STD_23 1
#else
#  define C_STD_23 0
#endif

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

#if C_STD_23
#  define alignment_from(...) alignof(typeof(*(__VA_ARGS__)))
#else
#  define alignment_from(...) m_alignment_from_((__VA_ARGS__))
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

CMYCOMMON_DEF size_t m_alignment_from_(void *ptr);

CMYCOMMON_DEF void *m_xmalloc_(size_t size, int line, const char *file);
CMYCOMMON_DEF void *m_xrealloc_(void *ptr, size_t new_size, int line, const char *file);
CMYCOMMON_DEF void *m_xcalloc_(size_t number, size_t size, int line, const char *file);

CMYCOMMON_DEF void m_panicf_(const char *file, int line, const char *fmt, ...);

/**
 * @file cmyallocator.h
 * @author Hesham Can't Fly
 * @brief Polymorphic Allocator Interace
 *
 * @note This header depends on cmycontext.h. I can't just #include it here because it will become recursive
 */

#include <stdlib.h>
#include <string.h>
#include <stdalign.h>


#ifndef CMYALLOCATOR_DEF
#  define CMYALLOCATOR_DEF
#endif /* !CMYALLOCATOR_DEF */

typedef struct allocator_interface_t {
	/** @brief Allocate memory.
	 *  @param self       Opaque state pointer.
	 *  @param file       The file where the function is called.
	 *  @param line       The line where the function is called.
	 *  @param alignment  Required alignment.
	 *  @param size       Number of bytes to allocate.
	 *  @return Pointer to the allocated memory, or NULL on failure. */
	void *(*allocate)(void *self, char *file, int line, size_t alignment, size_t size);
	/** @brief Reallocate memory (may move).
	 *  @param self       Opaque state pointer.
	 *  @param file       The file where the function is called.
	 *  @param line       The line where the function is called.
	 *  @param old_size   Previous allocation size.
	 *  @param ptr        Previous pointer.
	 *  @param alignment  Required alignment.
	 *  @param new_size   New size in bytes.
	 *  @return Pointer to the resized memory, or NULL on failure. */
	void *(*reallocate)(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size);
	/** @brief Free memory.
	 *  @param self  Opaque state pointer.
	 *  @param file  The file where the function is called.
	 *  @param line  The line where the function is called.
	 *  @param size  Size of the allocation.
	 *  @param ptr   Pointer to free. */
	void (*deallocate)(void *self, char *file, int line, size_t size, void *ptr);
} allocator_interface_t;

typedef struct allocator_t {
	const allocator_interface_t *vtable;
	void *data;
} allocator_t;

/**
 * @brief Allocate one item of type T_ using the allocator in the current `context`.
 *
 * # Example
 * @code
 * typedef struct person_t {
 *     const char *name;
 *     int age;
 * } person_t;
 *
 * int *a = new(int, {1});
 * person_t *me = new(person_t, { .name = "Hesham", .age = 21 });
 * @endcode
 *
 * @see create
 */
#define new(T_, ...) \
	(create((context.allocator), T_, __VA_ARGS__))

/**
 * @brief Allocates a slice of T_ with len_ using the allocator in the current `context`
 *
 * @see cmyslice.h
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * // In this case. You don't have to "cast" to `int_slice_t`. however, when passing to a function you'll need to cast.
 * int_slice_t a = (int_slice_t)new_slice(int, 10);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define new_slice(T_, len_) \
	create_slice((context.allocator), T_, len_)

/**
 * @brief Reallocate slice_ with context.allocator with new_len_
 *
 * @see cmyslice.h
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * int_slice_t ints = new_slice(int, 10);
 *
 * ints = (int_slice_t)renew_slice(ints, 15);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define renew_slice(slice_, new_len_) \
	recreate_slice((context.allocator), (slice_), (new_len_))

/**
 * @brief Releases ptr_ to the context allocator.
 *
 * # Example
 * @code
 * int *x = new(int, {1});
 * delete(x);
 * @endcode
 *
 * @see destroy
 */
#define delete(ptr_) destroy((context.allocator), ptr_)

/**
 * @brief Releases slice_ to the context allocator.
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * int_slice_t a = new_slice(int, 10);
 * delete_slice(a);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 * @see cmyslice.h
 */
#define delete_slice(slice_) \
	(destroy_slice(context.allocator, slice_))

/**
 * @brief Allocate one item of T_ using allocator_.
 *
 * # Example
 * @code
 * typedef struct person_t {
 *     const char *name;
 *     int age;
 * } person_t;
 *
 * allocator_t allocator = ...;
 * int *a = create(allocator, int, {1});
 * person_t *me = create(allocator, person_t, { .name = "Hesham", .age = 21 });
 * @endcode
 */
#define create(allocator_, T_, ...) \
	((T_ *)m_allocate_with_value_((allocator_), __FILE__, __LINE__, alignof(T_), sizeof(T_), &((T_) __VA_ARGS__)))

/**
 * @brief Allocates a slice of T_ with len_.
 *
 * @see cmyslice.h
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * allocator_t allocator = ...;
 *
 * // In this case. You don't have to "cast" to `int_slice_t`. however, when passing to a function you'll need to cast.
 * int_slice_t a = (int_slice_t)create_slice(allocator, int, 10);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define create_slice(allocator_, T_, len_) \
	{ .items = (allocate((allocator_), alignof(T_), ((sizeof(T_))*(len_)))), .len = (len_) }

/**
 * @brief Reallocate slice_ with allocator_ with new_len_
 *
 * @see cmyslice.h
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * allocator_t allocator = context.allocator;
 * int_slice_t ints = create_slice(allocator, int, 10);
 *
 * ints = (int_slice_t)recreate_slice(allocator, ints, 15);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define recreate_slice(allocator_, slice_, new_len_) \
	{ .items = (reallocate((allocator_), ((sizeof(*(slice_).items))*(slice_).len), (slice_).items, alignment_from((slice_).items), ((sizeof(*(slice_).items))*(new_len_)))), .len = (new_len_) }

/**
 * @brief Releases ptr_ to the context allocator_.
 *
 * # Example
 * @code
 * allocator_t allocator = ...;
 *
 * int *x = create(allocator, int, {1});
 * destroy(allocator, x);
 * @endcode
 */
#define destroy(allocator_, ptr_) \
	(deallocate((allocator_), (sizeof (*(ptr_))), (ptr_)))

/**
 * @brief Releases slice_ to allocator_.
 *
 * # Example
 * @code
 * typedef struct int_slice_t {
 *     size_t len;
 *     int *items;
 * } person_t;
 *
 * allocator_t allocator = ...;
 *
 * int_slice_t a = create_slice(allocator, int, {10});
 * destroy_slice(allocator, a);
 * @endcode
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 * @see cmyslice.h
 */
#define destroy_slice(allocator_, slice_) \
	(deallocate((allocator_), ((sizeof (*((slice_).items))) * ((slice_).len)), (slice_).items))

/**
 * @brief Allocate memory with size_ and alignment_.
 */
#define allocate(allocator_, alignment_, size_) \
	(m_allocate_((allocator_), __FILE__, __LINE__, (alignment_), (size_)))

/**
 * @brief Reallocate ptr_ of old_size_ with new_size_ and alignment_.
 */
#define reallocate(allocator_, old_size_, ptr_, alignment_, new_size_) \
	(m_reallocate_((allocator_), __FILE__, __LINE__, (old_size_), (ptr_), (alignment_), (new_size_)))

/**
 * @brief Releases ptr_ of size_ to the "heap".
 */
#define deallocate(allocator_, size_, ptr_) \
	(m_deallocate_((allocator_), __FILE__, __LINE__, (size_), (ptr_)))

CMYALLOCATOR_DEF void *m_allocate_(allocator_t allocator, char *file, int line, size_t alignment, size_t size);
CMYALLOCATOR_DEF void *m_reallocate_(allocator_t allocator, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size);
CMYALLOCATOR_DEF void m_deallocate_(allocator_t allocator, char *file, int line, size_t size, void *ptr);

CMYALLOCATOR_DEF void *m_allocate_with_value_(allocator_t allocator, char *file, int line, size_t alignment, size_t size, void *value);

/**
 * @file cmy_c_allocator.h
 * @author Hesham Can't Fly
 * @brief A wrapper on top of C's malloc/free for cmyallocator
 */

#include <stdlib.h>

#ifndef CMY_C_ALLOCATOR_DEF
#  define CMY_C_ALLOCATOR_DEF
#endif /* !CMY_C_ALLOCATOR_DEF */

CMY_C_ALLOCATOR_DEF allocator_t c_allocator(void);



#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct debug_allocator_location_t {
	char *file;
	int line;
} debug_allocator_location_t;

typedef struct debug_allocation_info_t {
	struct debug_allocation_info_t *next, *prev;
	size_t size;
	debug_allocator_location_t allocated_in;
	unsigned char data[];
} debug_allocation_info_t;

typedef struct debug_deallocation_info_t {
	struct debug_deallocation_info_t *next, *prev;
	size_t size;
	debug_allocator_location_t allocated_in, freed_in;
	void *ptr;
} debug_deallocation_info_t;

typedef struct debug_allocator_t {
	size_t total_allocated, // TODO: Consider renaming these to currently_allocated & currently_freed
	       total_freed,

	       total_double_free,
	       total_invalid_free,
	       total_invalid_realloc;
	bool panic_on_ub,
	     silence;
	debug_allocation_info_t *allocated_head;
	debug_allocation_info_t *allocated_tail;
	debug_deallocation_info_t *freed_head;
	debug_deallocation_info_t *freed_tail;
} debug_allocator_t;

debug_allocator_t make_debug_allocator(void);
size_t deinit_debug_allocator(debug_allocator_t *dbg);
allocator_t debug_allocator(debug_allocator_t *dbg);

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
	allocator_t allocator;
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
#define subslice(slice_, begin_, end_) \
	{ \
		.items = ((begin_ >= (slice_).len) ? (panicf("begin_ is > len: %zu", (slice_).len), NULL) : ((slice_).items + (begin_))), \
		.len = ((end_ >= (slice_).len) ? (panicf("end_ is > len: %zu", (slice_).len), 0) : ((end_) - (begin_))), \
	}

/**
 * @brief Iterate over each element in a slice. `break` and `continue` are also supported.
 *
 * # Example
 * @code
 * int_slice_t integers = slice(int, 1, 2, 3);
 * FORSLICE (int, item, integers) {
 *     printf("%d\n", item);
 * }
 * @endcode
 *
 * @param type_ Element type.
 * @param var_ Variable receiving the current element (by value).
 * @param slice_ Slice with `items` and `len` fields.
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
#define FORSLICE(type_, var_, slice_) \
	for (type_ *var_##_ptr_ = (slice_).items, \
	           *var_##_tmp_ = (void*)1, \
	           *var_##_end_ = (slice_).items + (slice_).len; \
		       (var_##_tmp_ == (void*)1) && var_##_ptr_ < var_##_end_; \
		       var_##_ptr_ += 1, var_##_tmp_ = (void*)(((char*)var_##_tmp_) + 1)) \
		for (type_ var_ = *var_##_ptr_; var_##_tmp_ == (void*)1; var_##_tmp_ = NULL)

/**
 * Iterate over each element in a slice, also providing its zero-based index.
 *
 * # Example
 * @code
 * int_slice_t integers = slice(int, 1, 2, 3);
 * IFORSLICE (int, i, item, integers) {
 *     printf("%z -> %d\n", i, item);
 * }
 * @endcode
 *
 * @param type_ Element type.
 * @param i_ Variable receiving the current element index. its going to be of type size_t.
 * @param var_ Variable receiving the current element (by value).
 * @param slice_ Slice with `items` and `len` fields.
 *
 * @warning Macro arguments are evaluated multiple times; avoid side effects (e.g., `i++`).
 */
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

CMYCOMMON_DEF size_t m_alignment_from_(void *ptr)
{
	uintptr_t p = (uintptr_t)ptr;

	size_t result = (size_t)(p & -p);
	if (result > alignof(max_align_t)) {
		return result;
	}

	return result;
}

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


CMYALLOCATOR_DEF void *m_allocate_(allocator_t allocator, char *file, int line, size_t alignment, size_t size)
{
	return (allocator.vtable->allocate)(allocator.data, file, line, alignment, size);
}

CMYALLOCATOR_DEF void *m_reallocate_(allocator_t allocator, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size)
{
	if (ptr == NULL) {
		return m_allocate_(allocator, file, line, alignment, new_size);
	}
	return (allocator.vtable->reallocate)(allocator.data, file, line, old_size, ptr, alignment, new_size);
}

CMYALLOCATOR_DEF void m_deallocate_(allocator_t allocator, char *file, int line, size_t size, void *ptr)
{
	(allocator.vtable->deallocate)(allocator.data, file, line, size, ptr);
}

CMYALLOCATOR_DEF void *m_allocate_with_value_(allocator_t allocator, char *file, int line, size_t alignment, size_t size, void *value)
{
	void *result = m_allocate_(allocator, file, line, alignment, size);
	memcpy(result, value, size);
	return result;
}


static void *c_allocate(void *self, char *file, int line, size_t alignment, size_t size);
static void *c_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size);
static void  c_deallocate(void *self, char *file, int line, size_t size, void *ptr);

static allocator_interface_t m_c_allocator_vtable_ = {
	.allocate = c_allocate,
	.reallocate = c_reallocate,
	.deallocate = c_deallocate,
};

CMY_C_ALLOCATOR_DEF allocator_t c_allocator(void)
{
	return (allocator_t) {
		.vtable = &m_c_allocator_vtable_,
		.data = NULL,
	};
}

static void *c_allocate(void *self, char *file, int line, size_t alignment, size_t size)
{
	(void)self;
	(void)file;
	(void)line;
	(void)alignment;
	(void)size;

	return malloc(size);
}

static void *c_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size)
{
	(void)self;
	(void)file;
	(void)line;
	(void)old_size;
	(void)ptr;
	(void)alignment;
	(void)new_size;

	return realloc(ptr, new_size);
}

static void c_deallocate(void *self, char *file, int line, size_t size, void *ptr)
{
	(void)self;
	(void)file;
	(void)line;
	(void)size;

	free(ptr);
}


static void *default_debug_allocate(void *self, char *file, int line, size_t alignment, size_t size);
static void *default_debug_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size);
static void  default_debug_deallocate(void *self, char *file, int line, size_t size, void *ptr);

static allocator_interface_t default_debug_allocator_vtable = {
	.allocate = default_debug_allocate,
	.reallocate = default_debug_reallocate,
	.deallocate = default_debug_deallocate,
};

debug_allocator_t make_debug_allocator(void)
{
	return (debug_allocator_t){0};
}

size_t deinit_debug_allocator(debug_allocator_t *dbg)
{
	if (dbg->total_allocated == 0) return 0;

	if (!dbg->silence) {
		fprintf(stderr, "Error: MEMORY LEAK DETECTED! Leaked Totally %zu bytes. Only Freed %zu bytes.\n", dbg->total_allocated, dbg->total_freed);
	}

	size_t amount = 0;
	for (debug_allocation_info_t *current = dbg->allocated_head; current != NULL; current = current->next) {
		if (!dbg->silence) {
			fprintf(stderr, "%s:%d: Error: %zu bytes leaked here. Address: %p.\n", current->allocated_in.file, current->allocated_in.line, current->size, (void*)current->data);
		}
		amount += 1;
		free(current);
	}

	for (debug_deallocation_info_t *current=dbg->freed_head; current != NULL; current = current->next) {
		free(current);
	}

	dbg->allocated_head = NULL;
	dbg->allocated_tail = NULL;
	dbg->freed_head = NULL;
	dbg->freed_tail = NULL;

	return amount;
}

allocator_t debug_allocator(debug_allocator_t *dbg)
{
	return (allocator_t){
		.vtable = &default_debug_allocator_vtable,
		.data = (void*)dbg,
	};
}

static void append_allocation(debug_allocator_t *dbg, debug_allocation_info_t *info)
{
	if (dbg->allocated_head == NULL) {
		dbg->allocated_head = info;
		dbg->allocated_tail = info;
	} else {
		info->prev = dbg->allocated_tail;
		dbg->allocated_tail->next = info;
		dbg->allocated_tail = info;
	}
}

static void remove_allocation(debug_allocator_t *dbg, debug_allocation_info_t *info)
{
	if (info == NULL) return;

	if (dbg->allocated_head == info) {
		dbg->allocated_head = info->next;
	}

	if (dbg->allocated_tail == info) {
		dbg->allocated_tail = info->prev;
	}

	if (info->prev != NULL) {
		info->prev->next = info->next;
	}

	if (info->next != NULL) {
		info->next->prev = info->prev;
	}
}

static void append_deallocation(debug_allocator_t *dbg, debug_deallocation_info_t *info)
{
	if (dbg->freed_head == NULL) {
		dbg->freed_head = info;
		dbg->freed_tail = info;
	} else {
		info->prev = dbg->freed_tail;
		dbg->freed_tail->next = info;
		dbg->freed_tail = info;
	}
}

static void remove_deallocation(debug_allocator_t *dbg, debug_deallocation_info_t *info)
{
	if (info == NULL) return;

	if (dbg->freed_head == info) {
		dbg->freed_head = info->next;
	}

	if (dbg->freed_tail == info) {
		dbg->freed_tail = info->prev;
	}

	if (info->prev != NULL) {
		info->prev->next = info->next;
	}

	if (info->next != NULL) {
		info->next->prev = info->prev;
	}
}

static bool check_if_allocated(debug_allocator_t *dbg, void *ptr)
{
	for (debug_allocation_info_t *current = dbg->allocated_head; current != NULL; current = current->next) {
		if ((void*)current->data == ptr) {
			return true;
		}
	}

	return false;
}

static debug_deallocation_info_t *check_if_already_freed(debug_allocator_t *dbg, void *ptr)
{
	for (debug_deallocation_info_t *current = dbg->freed_head; current != NULL; current = current->next) {
		if ((void*)current->ptr == ptr) {
			return current;
		}
	}

	return NULL;
}

// TODO: Move allocation and deallocation responsibilities to append_allocation/remove_allocation functions

static void *default_debug_allocate(void *self, char *file, int line, size_t alignment, size_t size)
{
	(void)alignment;
	debug_allocator_t *dbg = self;

	debug_allocation_info_t *result = malloc(sizeof(debug_allocation_info_t) + size);
	setzero(result);

	result->size = size;
	result->allocated_in.file = file;
	result->allocated_in.line = line;

	append_allocation(dbg, result);
	dbg->total_allocated += size;

	debug_deallocation_info_t *info = check_if_already_freed(dbg, (void*)result->data);
	remove_deallocation(dbg, info);
	free(info);

	return (void*)result->data;
}

// TODO: I should Consider doing a warning in case the old_size didn't match the actual size allocated
static void *default_debug_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size)
{
	(void)alignment;

	debug_allocator_t *dbg = self;

	if (ptr == NULL) {
		return default_debug_allocate(self, file, line, alignment, new_size);
	}

	if (!check_if_allocated(dbg, ptr)) {
		if (!dbg->silence) {
			fprintf(stderr, "%s:%d: Error: Cannot Reallocate %p as its not previously allocated.\n", file, line, ptr);
		}
		if (dbg->panic_on_ub) {
			exit(1);
		} else {
			dbg->total_invalid_realloc += 1;
			return NULL;
		}
	}

	{
		debug_deallocation_info_t *info = check_if_already_freed(dbg, ptr);
		if (info != NULL) {
			if (!dbg->silence) {
				fprintf(stderr, "%s:%d: Error: Cannot Reallocate %p as its already been freed.\n", file, line, ptr);
				fprintf(stderr, "%s:%d: Info: Its *already* freed here.\n", info->freed_in.file, info->freed_in.line);
				fprintf(stderr, "%s:%d: Info: Previously allocated here.\n", info->allocated_in.file, info->allocated_in.line);
			}
			if (dbg->panic_on_ub) {
				exit(1);
			} else {
				dbg->total_invalid_realloc += 1;
				return NULL;
			}
		}
	}

	debug_allocation_info_t *allocation_info = ((debug_allocation_info_t*)ptr)-1;
	debug_allocator_location_t allocated_in = allocation_info->allocated_in;
	debug_allocator_location_t deallocated_in = {
		.file = file,
		.line = line,
	};

	remove_allocation(dbg, allocation_info);

	debug_allocation_info_t *result = realloc(allocation_info, sizeof(debug_allocation_info_t) + new_size);
	setzero(result);

	result->size = new_size;
	result->allocated_in.file = file;
	result->allocated_in.line = line;

	if (allocation_info != result) {
		debug_deallocation_info_t *deallocation_info = malloc(sizeof(debug_deallocation_info_t));
		setzero(deallocation_info);

		deallocation_info->size = old_size;
		deallocation_info->allocated_in = allocated_in;
		deallocation_info->freed_in = deallocated_in;
		deallocation_info->ptr = ptr;

		append_deallocation(dbg, deallocation_info);
	}
	append_allocation(dbg, result);

	dbg->total_allocated -= old_size;
	dbg->total_allocated += new_size;

	return (void*)result->data;
}

static void default_debug_deallocate(void *self, char *file, int line, size_t size, void *ptr)
{
	debug_allocator_t *dbg = self;

	if (!check_if_allocated(dbg, ptr)) {
		if (!dbg->silence) {
			fprintf(stderr, "%s:%d: Error: Attempt at freeing invalid pointer: %p.\n", file, line, ptr);
		}
		if (dbg->panic_on_ub) {
			exit(1);
		} else {
			dbg->total_invalid_free += 1;
			return;
		}
	}

	{
		debug_deallocation_info_t *info = check_if_already_freed(dbg, ptr);
		if (info != NULL) {
			if (!dbg->silence) {
				fprintf(stderr, "%s:%d: Error: Double Free of %p has been detected!\n", file, line, ptr);
				fprintf(stderr, "%s:%d: Info: Its *already* freed here.\n", info->freed_in.file, info->freed_in.line);
				fprintf(stderr, "%s:%d: Info: Previously allocated here.\n", info->allocated_in.file, info->allocated_in.line);
			}
			if (dbg->panic_on_ub) {
				exit(1);
			} else {
				dbg->total_double_free += 1;
				return;
			}
		}
	}

	debug_allocation_info_t *allocation_info = ((debug_allocation_info_t*)ptr)-1;

	debug_allocator_location_t allocated_in = allocation_info->allocated_in;
	debug_allocator_location_t deallocated_in = {
		.file = file,
		.line = line,
	};

	remove_allocation(dbg, allocation_info);
	free(allocation_info);

	dbg->total_allocated -= size;
	dbg->total_freed += size;

	debug_deallocation_info_t *deallocation_info = malloc(sizeof(debug_deallocation_info_t));
	setzero(deallocation_info);

	deallocation_info->size = size;
	deallocation_info->allocated_in = allocated_in;
	deallocation_info->freed_in = deallocated_in;
	deallocation_info->ptr = ptr;

	append_deallocation(dbg, deallocation_info);
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
