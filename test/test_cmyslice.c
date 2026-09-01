#include "cmytest.h"

#define CMYCOMMON_IMPL
#include "cmycommon.h"
#include "cmyslice.h"

typedef struct int_slice_t {
	SLICE_TYPE(int);
} int_slice_t;

TEST_CASE(first_slicing_test)
{
	int numbers[] = {1, 2, 3};

	int_slice_t ints = slice(int, 1, 2, 3);
	TEST_ASSERT(ints.len == 3);
	TEST_ASSERT(get(ints, 0) == 1);
	TEST_ASSERT(get(ints, 1) == 2);
	TEST_ASSERT(get(ints, 2) == 3);

	int_slice_t ints2 = slice_from_ptr(numbers, 3);
	TEST_ASSERT(ints2.items == numbers);
	TEST_ASSERT(ints2.len == 3);

	TEST_PASS();
}

TEST_CASE(test_slicing)
{
	int_slice_t ints = slice(int, 1, 2, 3, 4, 5, 6);

	int_slice_t other_slice = slicing(ints, 1, 4);
	TEST_ASSERT(other_slice.len == 3);
	TEST_ASSERT(other_slice.items == rget(ints, 1)); // rget returns a pointer

	TEST_ASSERT(get(other_slice, 0) == 2);
	TEST_ASSERT(get(other_slice, 1) == 3);
	TEST_ASSERT(get(other_slice, 2) == 4);
	
	TEST_PASS();
}

TEST_CASE(test_looping)
{
	int_slice_t ints = slice(int, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	const int expected_total = 55;
	const int expected_even = 25;
	const int expected_odd = 30;
	const int expected_to_5 = 10;

	{
		int calculated = 0;
		FORSLICE (int, item, ints) {
			calculated += item;
		}
		TEST_ASSERT(calculated == expected_total);
	}

	{
		int calculated = 0;
		FORSLICE (int, item, ints) {
			if (item % 2 == 0) {
				continue;
			}
			calculated += item;
		}
		TEST_ASSERT(calculated == expected_even);
	}

	{
		int calculated = 0;
		FORSLICE (int, item, ints) {
			if (item % 2 != 0) {
				continue;
			}
			calculated += item;
		}
		TEST_ASSERT(calculated == expected_odd);
	}

	{
		int calculated = 0;
		FORSLICE (int, item, ints) {
			if (item == 5) {
				break;
			}
			calculated += item;
		}
		TEST_ASSERT(calculated == expected_to_5);
	}

	TEST_PASS();
}

TEST_CASE(test_indexed_looping)
{
	int_slice_t ints = slice(int, 2, 4, 6, 8, 10);
	const int expected_total_values = 30;
	const size_t expected_total_idx = 10;

	const int expected_value_even_idx = 2 + 6 + 10;
	const size_t expected_idx_even_idx = 0 + 2 + 4;
	
	const int expected_value_upto_6 = 6;
	const int expected_idx_upto_6 = 1;

	{
		size_t calculated_idx = 0;
	    int calculated_value = 0;
		IFORSLICE (int, i, item, ints) {
			calculated_value += item;
			calculated_idx += i;
		}

		TEST_ASSERT(calculated_idx == expected_total_idx);
		TEST_ASSERT(calculated_value == expected_total_values);
	}

	{
		size_t calculated_idx = 0;
	    int calculated_value = 0;
		IFORSLICE (int, i, item, ints) {
			if (i % 2 != 0) {
				continue;
			}
			calculated_value += item;
			calculated_idx += i;
		}

		TEST_ASSERT(calculated_idx == expected_idx_even_idx);
		TEST_ASSERT(calculated_value == expected_value_even_idx);
	}

	{
		size_t calculated_idx = 0;
	    int calculated_value = 0;
		IFORSLICE (int, i, item, ints) {
			if (item == 6) {
				break;
			}
			calculated_value += item;
			calculated_idx += i;
		}

		TEST_ASSERT(calculated_idx == expected_idx_upto_6);
		TEST_ASSERT(calculated_value == expected_value_upto_6);
	}

	TEST_PASS();
}

int main(void)
{
	TEST_SETUP();

	RUN_TEST(first_slicing_test);
	RUN_TEST(test_slicing);
	RUN_TEST(test_looping);
	RUN_TEST(test_indexed_looping);

	TEST_SUMMARY();
}
