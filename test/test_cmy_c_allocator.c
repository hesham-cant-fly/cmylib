#include "cmytest.h"

#define CMYALLOCATOR_IMPL
#include "cmyallocator.h"

#define CMYCONTEXT_IMPL
#include "cmycontext.h"

// I'll assume that this works
#define CMY_C_ALLOCATOR_IMPL
#include "cmy_c_allocator.h"

TEST_CASE(test_cmy_c_allocator)
{
	int *x = new(int, {67});

	TEST_ASSERT(*x == 67);

	delete(x);
	TEST_PASS();
}

int main(void)
{
	init_context();
	context.allocator = c_allocator();

	TEST_SETUP();

	RUN_TEST(test_cmy_c_allocator);

	TEST_SUMMARY();
}
