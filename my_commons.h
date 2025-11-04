#ifndef __MY_COMMONS_H
#define __MY_COMMONS_H

// #define MY_COMMONS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdio.h>

#define unused(__x) ((void)(__x))
#define panic(__fmt) panic(__FILE__, __LINE__, (__fmt))
#define unreachable() __unreachable(__FILE__, __LINE__)

void __panic(const char *file, int line, const char *restrict fmt, ...);
void __unreachable(const char *file, int line);

#ifdef MY_COMMONS_IMPLEMENTATION
void __panic(const char *file, int line, const char *restrict fmt, ...) {
  fprintf(stderr, "%s:%d: Error: Panic: %s.\n", file, line, fmt);
  exit(69);
}

void __unreachable(const char *file, int line) {
  fprintf(stderr, "%s:%d: Error: reached an unreachable.\n", file, line);
  exit(1);
}
#endif // MY_COMMONS_IMPLEMENTATION

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !__MY_COMMONS_H
