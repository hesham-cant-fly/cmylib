#ifndef CMYCONTEXT_H
#define CMYCONTEXT_H

/* #define CMYCONTEXT_IMPL */

#include <stddef.h>
#include <stdlib.h>

#define context (m_context_stack_.items[m_context_stack_.len - 1])
#define PUSH_CONTEXT() for (int i_=(push_context(), 1); i_; i_=(pop_context(), 0))

typedef struct context_t {
	size_t id;
} context_t;

typedef struct context_stack_t {
	size_t len, cap;
	context_t *items;
} context_stack_t;

extern context_t *m_current_context_;
extern context_stack_t m_context_stack_;

void init_context(void);

void push_context(void);
void pop_context(void);


#ifdef CMYCONTEXT_IMPL

context_stack_t m_context_stack_ = {0};

void init_context(void)
{
	push_context();
}

void push_context(void)
{
	if (m_context_stack_.len >= m_context_stack_.cap) {
		m_context_stack_.cap += 20; // adding 20 because I think exponontial growth is not fitting here
		// FIXME: UB!! You can't pass a NULL to realloc
		m_context_stack_.items = realloc(m_context_stack_.items, m_context_stack_.cap);
	}

	m_context_stack_.len += 1;
	m_context_stack_.items[m_context_stack_.len - 1] = m_context_stack_.items[m_context_stack_.len - 2]
}

void pop_context(void)
{
	if (m_context_stack_.len < 0) {
		return;
	}

	m_context_stack_.len -= 1;
}

#endif /* CMYCONTEXT_IMPL_IMPL */

#endif /* CMYCONTEXT_H */
