#ifndef MY_STRING_H_
#define MY_STRING_H_

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef GROW_FACTOR
#define GROW_FACTOR 1.5
#endif

typedef struct String {
    char *data;
    size_t len;
    size_t cap;
} String;

#ifdef MY_STRING_IMPL
static inline size_t string_actual_cap(const String *self) {
    return self->cap + 1;
}

static inline size_t string_actual_len(const String *self) {
    return self->len + 1;
}
#endif

String string_from_chars(const char *chs)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(chs);
    return (String){(char *)chs, len, len};
}
#else
    ;
#endif
String string_from_chars_copy(const char *chs)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(chs);
    String res = {
        .cap = len,
        .len = len,
        .data = strdup(chs),
    };

    return res;
}
#else
    ;
#endif
String string_new(const size_t cap)
#ifdef MY_STRING_IMPL
{
    String res = {
        .cap = cap,
        .len = 0,
        .data = alloc(char, cap + 1),
    };
    res.data[0] = '\0';
    return res;
}
#else
    ;
#endif

const char *string_as_cstr(const String *self)
#ifdef MY_STRING_IMPL
{
    return self->data;
}
#else
    ;
#endif
size_t string_set_len(String *self, size_t len)
#ifdef MY_STRING_IMPL
{
    self->len = len;
    self->data[len] = '\0';
    return self->len;
}
#else
    ;
#endif

String string_format(const char *fmt, ...)
#ifdef MY_STRING_IMPL
{
    va_list args;

    va_start(args, fmt);
    size_t len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    String result = string_new(len);

    va_start(args, fmt);
    vsnprintf(result.data, len + 1, fmt, args);
    va_end(args);

    result.len = len;
    return result;
}
#else
    ;
#endif

void string_reserve(String *self, const size_t new_cap)
#ifdef MY_STRING_IMPL
{
    self->data = realloc(self->data, new_cap + 1);
    self->cap = new_cap;

    if (self->data == NULL) {
        panic(("Cannot reserve new capacity."));
    }
}
#else
    ;
#endif
void string_resize(String *self, const size_t new_size)
#ifdef MY_STRING_IMPL
{
    if (self->cap < new_size) {
        self->data = realloc(self->data, new_size + 1);
    }

    if (self->len < new_size) {
        memset(self->data + self->len, 0, new_size - self->len);
    }

    self->len = new_size;
    self->data[self->len] = '\0';
}
#else
    ;
#endif
void string_delete(String *self)
#ifdef MY_STRING_IMPL
{
    self->len = 0;
    self->cap = 0;
    free(self->data);
}
#else
    ;
#endif

void string_push_char(String *self, const char ch)
#ifdef MY_STRING_IMPL
{
    if (self->len + 1 >= self->cap) {
        string_reserve(self, self->cap * GROW_FACTOR);
    }

    self->data[self->len++] = ch;
    self->data[self->len] = '\0';
}
#else
    ;
#endif
void string_push_cstr(String *self, const char *cstr)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(cstr);
    if (self->len + len >= self->cap) {
        string_reserve(self, self->cap * GROW_FACTOR);
    }

    memcpy(self->data + self->len, cstr, len);
    self->len += len;
    self->data[self->len] = '\0';
}
#else
    ;
#endif
void string_push_string(String *self, const String *other)
#ifdef MY_STRING_IMPL
{
    if (self->len + other->len >= self->cap) {
        string_reserve(self, self->cap * GROW_FACTOR);
    }

    memcpy(self->data + self->len, other->data, other->len);
    self->len += other->len;
    self->data[self->len] = '\0';
}
#else
    ;
#endif
#define string_push(self, target)                                              \
    _Generic((target),                                                         \
        char: string_push_char,                                                \
        char *: string_push_cstr,                                              \
        String *: string_push_string)(self, target)

bool string_eq_cstr(const String *self, const char *other)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(other);
    if (self->len != len)
        return false;

    return strncmp(self->data, other, len) == 0;
}
#else
    ;
#endif
bool string_eq_string(const String *self, const String *other)
#ifdef MY_STRING_IMPL
{
    if (self->len != other->len)
        return false;

    return strncmp(self->data, other->data, self->len) == 0;
}
#else
    ;
#endif
#define string_eq(self, target)                                                \
    _Generic((target), char *: string_eq_cstr, String *: string_eq_string)(    \
        self, target)

bool string_starts_with_char(const String *self, const char other)
#ifdef MY_STRING_IMPL
{
    if (self->len != 1)
        return false;

    return self->data[0] == other;
}
#else
    ;
#endif
bool string_starts_with_cstr(const String *self, const char *other)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(other);
    if (self->len < len)
        return false;

    return strncmp(self->data, other, len) == 0;
}
#else
    ;
#endif
bool string_starts_with_string(const String *self, const String *other)
#ifdef MY_STRING_IMPL
{
    if (self->len < other->len)
        return false;

    return strncmp(self->data, other->data, self->len);
}
#else
    ;
#endif
#define string_starts_with(self, other)                                        \
    _Generic((other),                                                          \
        char: string_starts_with_char,                                         \
        char *: string_starts_with_cstr,                                       \
        String *: string_starts_with_string)(self, other)

bool string_ends_with_char(const String *self, const char other)
#ifdef MY_STRING_IMPL
{
    if (self->len < 1)
        return false;

    return self->data[self->len - 1] == other;
}
#else
    ;
#endif
bool string_ends_with_cstr(const String *self, const char *other)
#ifdef MY_STRING_IMPL
{
    size_t len = strlen(other);
    if (self->len < len)
        return false;

    return strncmp(self->data + self->len - len, other, len) == 0;
}
#else
    ;
#endif
bool string_ends_with_string(const String *self, const String *other)
#ifdef MY_STRING_IMPL
{
    if (self->len < other->len)
        return false;

    return strncmp(self->data + self->len - other->len, other->data,
                   other->len) == 0;
}
#else
    ;
#endif
#define string_ends_with(self, other)                                          \
    _Generic((other),                                                          \
        char: string_ends_with_char,                                           \
        char *: string_ends_with_cstr,                                         \
        String *: string_ends_with_string)(self, other)

#endif // MY_STRING_H_
