#include "cmytest.h"
#define CMYCONTEXT_IMPL
#include "cmycontext.h"

TEST_CASE(test_context_init)
{
	init_context();

	TEST_ASSERT(m_context_stack_.cap == 20);
	TEST_ASSERT(m_context_stack_.len == 1);
	TEST_ASSERT(m_context_stack_.items != NULL);
	
	TEST_PASS();
}

int main(void)
{
	TEST_SETUP();

	RUN_TEST(test_context_init);
	
	TEST_SUMMARY();
}
