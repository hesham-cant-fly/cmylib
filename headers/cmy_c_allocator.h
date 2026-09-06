/**
 * @file cmy_c_allocator.h
 * @author Hesham Can't Fly
 * @brief A wrapper on top of C's malloc/free for cmyallocator
 */

#ifndef CMY_C_ALLOCATOR_H
#define CMY_C_ALLOCATOR_H

/* #define CMY_C_ALLOCATOR_IMPL */



#include <stdlib.h>
#include "cmyallocator.h"

#ifndef CMY_C_ALLOCATOR_DEF
#  define CMY_C_ALLOCATOR_DEF
#endif /* !CMY_C_ALLOCATOR_DEF */

CMY_C_ALLOCATOR_DEF allocator_t c_allocator(void);


#ifdef CMY_C_ALLOCATOR_IMPL

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

#endif /* CMY_C_ALLOCATOR_IMPL */

#endif /* CMY_C_ALLOCATOR_H */
