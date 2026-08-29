
CMYCONTEXT_DEF context_stack_t m_context_stack_ = {0};

CMYCONTEXT_DEF void init_context(void)
{
	push_context();
}

CMYCONTEXT_DEF void push_context(void)
{
	if (m_context_stack_.len >= m_context_stack_.cap) {
		m_context_stack_.cap += 20; // adding 20 because I think exponontial growth is not fitting here
		// FIXME: UB!! You can't pass a NULL to realloc
		m_context_stack_.items = realloc(m_context_stack_.items, m_context_stack_.cap);
	}

	m_context_stack_.len += 1;
	m_context_stack_.items[m_context_stack_.len - 1] = m_context_stack_.items[m_context_stack_.len - 2];
}

CMYCONTEXT_DEF void pop_context(void)
{
	if (m_context_stack_.len < 0) {
		return;
	}

	m_context_stack_.len -= 1;
}
