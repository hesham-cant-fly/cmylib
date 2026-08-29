/**
 * @file cmycontext.h
 * @author Hesham Can't Fly
 * @brief Context system similar to Odin's context
 * define CMYCONTEXT_DEF to static before including cmycontext.h if you want internal linkage.
 * # Example
 * @code
 * #define CMYCONTEXT_IMPL
 * #include "cmycontext.h"
 * #include <stdio.h>
 *
 * int main(void)
 * {
 *    init_context();
 *    context.id = 1;
 *    printf("%zu\n", context.id);
 *    PUSH_CONTEXT() {
 *        context.id += 1;
 *        printf("%zu\n", context.id);
 *    }
 * }
 * @endcode
 */
#include <stddef.h>
#include <stdlib.h>

#ifndef CMYCONTEXT_DEF
#  define CMYCONTEXT_DEF
#endif

/**
 * @brief access the current context
 */
#define context (m_context_stack_.items[m_context_stack_.len - 1])

/**
 * @brief push a new context and opens a new block. at the end it pops the context. it can be used like how you would use an if statement.
 * @warning doing `return` or a `goto` while inside this `PUSH_CONTEXT` block is not safe. as they don't allow for poping the context. if you want to do an early exit. use `continue;` or call `pop_context` before `return` or `goto`
 * @see push_context
 * @see pop_context
 */
#define PUSH_CONTEXT() for (int i_=(push_context(), 1); i_; i_=(pop_context(), 0))

/**
 * @brief a context type. feel free to modify it however you like.
 */
typedef struct context_t {
	size_t id;
} context_t;

typedef struct context_stack_t {
	size_t len, cap;
	context_t *items;
} context_stack_t;

extern context_t *m_current_context_;
extern context_stack_t m_context_stack_;

/**
 * @brief inittializes the context. call it only once at the top of `main`
 */
CMYCONTEXT_DEF void init_context(void);

/**
 * @brief pushes a new context
 */
CMYCONTEXT_DEF void push_context(void);

/**
 * @brief pops a context
 */
CMYCONTEXT_DEF void pop_context(void);
