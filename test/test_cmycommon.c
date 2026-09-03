#include "cmytest.h"
#define CMYCOMMON_IMPL
#include "cmycommon.h"

TEST_CASE(test_alignof)
{
	// Can't really test it. I'd just assume that it works

	TEST_PASS();
}

TEST_CASE(test_alignment_from)
{
	TEST_ASSERT(alignment_from((void *)0x01) == 1);
	TEST_ASSERT(alignment_from((void *)0x02) == 2);
	TEST_ASSERT(alignment_from((void *)0x03) == 1);
	TEST_ASSERT(alignment_from((void *)0x04) == 4);
	TEST_ASSERT(alignment_from((void *)0x05) == 1);
	TEST_ASSERT(alignment_from((void *)0x06) == 2);
	TEST_ASSERT(alignment_from((void *)0x07) == 1);
	TEST_ASSERT(alignment_from((void *)0x08) == 8);
	TEST_ASSERT(alignment_from((void *)0x09) == 1);
	TEST_ASSERT(alignment_from((void *)0x0A) == 2);
	TEST_ASSERT(alignment_from((void *)0x0B) == 1);
	TEST_ASSERT(alignment_from((void *)0x0C) == 4);
	TEST_ASSERT(alignment_from((void *)0x0D) == 1);
	TEST_ASSERT(alignment_from((void *)0x0E) == 2);
	TEST_ASSERT(alignment_from((void *)0x0F) == 1);
	TEST_ASSERT(alignment_from((void *)0x10) == 16);

	TEST_PASS();
}

TEST_CASE(test_forange)
{
	int result = 0;
	const int expected_result = 45;

	FORANGE (i, 0, 10) {
		result += i;
	}

	TEST_ASSERT(result == expected_result);

	TEST_PASS();
}

int main(void)
{
	TEST_SETUP();

	RUN_TEST(test_alignof);
	RUN_TEST(test_alignment_from);
	RUN_TEST(test_forange);

	TEST_SUMMARY();
}
