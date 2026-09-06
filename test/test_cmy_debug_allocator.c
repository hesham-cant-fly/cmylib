#define CMYLIB_IMPL
#include "cmylib.h"

typedef struct ints_slice_t {
	size_t len;
	int *items;
} ints_slice_t;

debug_allocator_t dbg_allocator = {0};

static void setup_debug_allocator(void);

TEST_CASE(test_debug_good_path)
{
	setup_debug_allocator();

	int *x = new(int, {67});

	TEST_ASSERT(*x == 67);
	TEST_ASSERT(dbg_allocator.total_allocated == sizeof(int));

	x = reallocate(context.allocator, sizeof(int), x, alignof(int), sizeof(int)*2);

	TEST_ASSERT(*x == 67);
	TEST_ASSERT(dbg_allocator.total_allocated == sizeof(int)*2);

	deallocate(context.allocator, sizeof(int)*2, x);
	TEST_ASSERT(dbg_allocator.total_allocated == 0);
	TEST_ASSERT(dbg_allocator.total_freed == sizeof(int)*2);

	size_t leak_count = deinit_debug_allocator(&dbg_allocator);
	TEST_ASSERT(leak_count == 0);

	TEST_PASS();
}

TEST_CASE(test_direct_memory_leak)
{
	setup_debug_allocator();

	int *x = new(int, {6});

	delete(x);

	new(int, {7});

	size_t object_leaked = deinit_debug_allocator(&dbg_allocator);
	TEST_ASSERT(object_leaked == 1);

	TEST_PASS();
}

int main(void)
{
	init_context();
	TEST_SETUP();

	RUN_TEST(test_debug_good_path);
	RUN_TEST(test_direct_memory_leak);

	TEST_SUMMARY();
}

static void setup_debug_allocator(void)
{
	dbg_allocator = make_debug_allocator();
	dbg_allocator.panic_on_ub = false;
	dbg_allocator.silence = true;
	context.allocator = debug_allocator(&dbg_allocator);
}
