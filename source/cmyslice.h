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
