
#include "cmycommon.h"
#include "cmyallocator.h"

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
