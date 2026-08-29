/**
 * @file cmytest.h
 * @author Hesham Can't Fly
 * @brief Super simple testing framework
 * @note If you want verbose output do `#define TEST_VERBOSE` before include cmytest.h
 * # Example
 * @code
 * #define TEST_VERBOSE // Enable verbose output
 * #include "cmytest.h"
 * // Everything you need to know is in this example
 * int add(int x, int y)
 * {
 *     return x + y
 * }
 *
 * TEST_CASE(add_test)
 * {
 *     TEST_ASSERT(add(1, 2) == 3);
 *     TEST_PASS();
 * }
 *
 * int main(void)
 * {
 *     RUN_TEST(add_test);
 *     TEST_SUMMARY();
 * }
 * @endcode
 * @todo I wrote this header before I wrote CONVENTIONS.org .. so maybe I should update some stuff in here lol.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define TEST_CLR_RESET   "\033[0m"
#define TEST_CLR_RED     "\033[31;1m"
#define TEST_CLR_GREEN   "\033[32;1m"
#define TEST_CLR_CYAN    "\033[36m"

#define TEST_PASS() return true
#define TEST_FAIL() return false

#define TEST_SETUP() \
	size_t total_tests = 0, failed_tests = 0

#ifdef TEST_VERBOSE
# define REPORT_TEST_ASSERT() \
	fprintf(stderr, "    [ %sASSERT%s    ] %s%40s%s -- (%s:%u)\n", TEST_CLR_CYAN, TEST_CLR_RESET, TEST_CLR_CYAN, __func__, TEST_CLR_RESET, __FILE__, __LINE__)
#else
# define REPORT_TEST_ASSERT()
#endif /* TEST_VERBOSE */
#define REPORT_TEST_ASSERT_FAILURE(...) \
	fprintf(stderr, "    [      %sFAIL%s ] %s%40s%s -- %s%s%s (%s:%u)\n", TEST_CLR_RED, TEST_CLR_RESET, TEST_CLR_CYAN, __func__, TEST_CLR_RESET, TEST_CLR_CYAN, #__VA_ARGS__, TEST_CLR_RESET, __FILE__, __LINE__)
#define REPORT_TEST_CASE_FAILURE(name_) \
	fprintf(stderr, "[      %sFAIL%s ] %s%44s%s -- (%s:%u)\n", TEST_CLR_RED, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)
#ifdef TEST_VERBOSE
# define REPORT_TEST_CASE(name_) \
	fprintf(stderr, "[ %sTEST%s      ] %s%44s%s -- (%s:%u)\n", TEST_CLR_GREEN, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)
# else
# define REPORT_TEST_CASE(name_)
#endif /* TEST_VERBOSE */
#define REPORT_TEST_CASE_OK(name_) \
	fprintf(stderr, "[ %sOK%s        ] %s%44s%s -- (%s:%u)\n", TEST_CLR_GREEN, TEST_CLR_RESET, TEST_CLR_CYAN, #name_, TEST_CLR_RESET, __FILE__, __LINE__)

#define TEST_SUMMARY() \
	do { \
		fprintf(stderr, "\n[  %sSUMMARY%s  ] %s%zu Total%s -- %s%zu Failed%s\n", TEST_CLR_CYAN, TEST_CLR_RESET, TEST_CLR_GREEN, total_tests, TEST_CLR_RESET, TEST_CLR_RED, failed_tests, TEST_CLR_RESET); \
		return failed_tests == 0 ? 0 : 1; \
	} while (0)

#define TEST_CASE(name_) static bool name_(void)
#define RUN_TEST(name_) \
	do { \
		total_tests += 1; \
		REPORT_TEST_CASE(name_); \
		const bool result__ = name_(); \
		if (!result__) { \
			failed_tests += 1; \
			REPORT_TEST_CASE_FAILURE(name_); \
		} else { \
			REPORT_TEST_CASE_OK(name_); \
		} \
	} while (0)

#define TEST_ASSERT(...) \
	do { \
		REPORT_TEST_ASSERT(); \
		const bool result__ = (__VA_ARGS__); \
		if (!result__) { \
			REPORT_TEST_ASSERT_FAILURE(__VA_ARGS__); \
			TEST_FAIL(); \
		} \
	} while (0)
