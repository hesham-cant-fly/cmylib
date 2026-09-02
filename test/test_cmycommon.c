#include "cmytest.h"
#define CMYCOMMON_IMPL
#include "cmycommon.h"

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

	RUN_TEST(test_forange);

	TEST_SUMMARY();
}
