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
