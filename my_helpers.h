#ifndef MY_HELPERS_H_
#define MY_HELPERS_H_

#define defer(value)                                                           \
    do {                                                                       \
        result = value;                                                        \
        goto defer;                                                            \
    } while (0)

#endif // MY_HELPERS_H_
