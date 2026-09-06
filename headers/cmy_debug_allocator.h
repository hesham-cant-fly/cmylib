#ifndef CMY_DEBUG_ALLOCATOR_H
#define CMY_DEBUG_ALLOCATOR_H

/* #define CMY_DEBUG_ALLOCATOR_IMPL */


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
	bool panic_on_ub;
	bool silence_ub;
	debug_allocation_info_t *allocated_head;
	debug_allocation_info_t *allocated_tail;
	debug_deallocation_info_t *freed_head;
	debug_deallocation_info_t *freed_tail;
} debug_allocator_t;

debug_allocator_t make_debug_allocator(void);
size_t deinit_debug_allocator(debug_allocator_t *dbg);
allocator_t debug_allocator(debug_allocator_t *dbg);


#ifdef CMY_DEBUG_ALLOCATOR_IMPL

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

	fprintf(stderr, "Error: MEMORY LEAK DETECTED! Leaked Totally %zu bytes. Only Freed %zu bytes.\n", dbg->total_allocated, dbg->total_freed);

	size_t amount = 0;
	for (debug_allocation_info_t *current = dbg->allocated_head; current != NULL; current = current->next) {
		fprintf(stderr, "%s:%d: Error: %zu bytes leaked here. Address: %p.\n", current->allocated_in.file, current->allocated_in.line, current->size, (void*)current->data);
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
		if (!dbg->silence_ub) {
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
			if (!dbg->silence_ub) {
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
		if (!dbg->silence_ub) {
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
			if (!dbg->silence_ub) {
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

#endif /* CMY_DEBUG_ALLOCATOR_IMPL */

#endif /* CMY_DEBUG_ALLOCATOR_H */
