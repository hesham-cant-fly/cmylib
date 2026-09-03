/**
 * @file cmyallocator.h
 * @author Hesham Can't Fly
 * @brief Polymorphic Allocator Interace
 *
 * @note This header depends on cmycontext.h. I can't just #include it here because it will become recursive
 */

#ifndef CMYALLOCATOR_H
#define CMYALLOCATOR_H

/* #define CMYALLOCATOR_IMPL */



#include <stdlib.h>
#include <string.h>
#include <stdalign.h>

#include "cmycommon.h"

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


#ifdef CMYALLOCATOR_IMPL

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

#endif /* CMYALLOCATOR_IMPL */

#endif /* CMYALLOCATOR_H */
