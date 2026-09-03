/**
 * @file cmy_c_allocator.h
 * @author Hesham Can't Fly
 * @brief A wrapper on top of C's malloc/free for cmyallocator
 */

#include <stdlib.h>
#include "cmyallocator.h"

#ifndef CMY_C_ALLOCATOR_DEF
#  define CMY_C_ALLOCATOR_DEF
#endif /* !CMY_C_ALLOCATOR_DEF */

CMY_C_ALLOCATOR_DEF allocator_t get_c_allocator(void);
