#include "cmytest.h"

#define CMYCOMMON_IMPL
#include "cmycommon.h"

#include "cmyslice.h"

#define CMYALLOCATOR_IMPL
#include "cmyallocator.h"

#define CMYCONTEXT_IMPL
#include "cmycontext.h"

void *custom_allocate(void *self, char *file, int line, size_t alignment, size_t size);
void *custom_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size);
void custom_deallocate(void *self, char *file, int line, size_t size, void *ptr);

size_t allocated = 0;

typedef struct int_slice_t {
	SLICE_TYPE(int);
} int_slice_t;

const allocator_interface_t vtable = {
	.allocate = custom_allocate,
	.reallocate = custom_reallocate,
	.deallocate = custom_deallocate,
};

TEST_CASE(test_new)
{
	TEST_ASSERT(allocated == 0);

	int *x = new(int, {1});

	TEST_ASSERT(*x == 1);
	TEST_ASSERT(allocated == sizeof(int));

	delete(x);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

TEST_CASE(test_create)
{
	TEST_ASSERT(allocated == 0);

	allocator_t allocator = context.allocator;

	int *x = create(allocator, int, {1});

	TEST_ASSERT(*x == 1);
	TEST_ASSERT(allocated == sizeof(int));

	destroy(allocator, x);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

TEST_CASE(test_new_slice)
{
	TEST_ASSERT(allocated == 0);

	int_slice_t ints = new_slice(int, 10);

	TEST_ASSERT(ints.len == 10);
	TEST_ASSERT(allocated == sizeof(int)*10);

	const int expected_result = 90;
	FORANGE (i, 0, ints.len) {
		set(ints, i, i * 2);
	}

	{
		int addition = 0;
		FORSLICE (int, item, ints) {
			addition += item;
		}

		TEST_ASSERT(addition == expected_result);
	}

	delete_slice(ints);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

TEST_CASE(test_create_slice)
{
	TEST_ASSERT(allocated == 0);

	allocator_t allocator = context.allocator;
	int_slice_t ints = create_slice(allocator, int, 10);

	TEST_ASSERT(ints.len == 10);
	TEST_ASSERT(allocated == sizeof(int)*10);

	const int expected_result = 90;
	FORANGE (i, 0, ints.len) {
		set(ints, i, i * 2);
	}

	{
		int addition = 0;
		FORSLICE (int, item, ints) {
			addition += item;
		}

		TEST_ASSERT(addition == expected_result);
	}

	destroy_slice(allocator, ints);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

TEST_CASE(test_allocate)
{
	TEST_ASSERT(allocated == 0);

	int *ptr = allocate(context.allocator, alignof(int), sizeof(int)*10);

	TEST_ASSERT(allocated == sizeof(int)*10);

	deallocate(context.allocator, sizeof(int)*10, ptr);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

TEST_CASE(test_reallocate)
{
	TEST_ASSERT(allocated == 0);

	int *ptr = allocate(context.allocator, alignof(int), sizeof(int));

	TEST_ASSERT(allocated == sizeof(int));

	ptr = reallocate(context.allocator, sizeof(int), ptr, alignof(int), sizeof(int)*10);
	TEST_ASSERT(allocated == sizeof(int)*10);

	ptr = reallocate(context.allocator, sizeof(int)*10, ptr, alignof(int), sizeof(int)*5);
	TEST_ASSERT(allocated == sizeof(int)*5);

	deallocate(context.allocator, sizeof(int)*5, ptr);
	TEST_ASSERT(allocated == 0);

	TEST_PASS();
}

int main(void)
{
	init_context();
	context.allocator = (allocator_t) {
		.vtable = &vtable,
		.data = NULL,
	};

	TEST_SETUP();

	RUN_TEST(test_new);
	RUN_TEST(test_create);
	RUN_TEST(test_new_slice);
	RUN_TEST(test_create_slice);
	RUN_TEST(test_allocate);
	RUN_TEST(test_reallocate);

	TEST_SUMMARY();
}

void *custom_allocate(void *self, char *file, int line, size_t alignment, size_t size)
{
	unused(self);
	unused(file);
	unused(line);
	unused(alignment);

	allocated += size;

	return malloc(size);
}

void *custom_reallocate(void *self, char *file, int line, size_t old_size, void *ptr, size_t alignment, size_t new_size)
{
	unused(self);
	unused(file);
	unused(line);
	unused(old_size);
	unused(ptr);
	unused(alignment);

	allocated -= old_size;
	allocated += new_size;

	return realloc(ptr, new_size);
}

void custom_deallocate(void *self, char *file, int line, size_t size, void *ptr)
{
	unused(self);
	unused(file);
	unused(line);
	unused(size);

	allocated -= size;

	free(ptr);
}
