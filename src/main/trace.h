#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef USE_TRACE

extern int level;

#define INDENT                        \
    do {                              \
        printf("%*s", level * 2, ""); \
    } while(false)

#define ENTER                                       \
    do {                                            \
        INDENT;                                     \
        printf("ENTER: %s:%d", __func__, __LINE__); \
        level++;                                    \
        fputc('\n', stdout);                        \
    } while(false)

#define RETURN(...)                                    \
    do {                                               \
        level--;                                       \
        INDENT;                                        \
        printf("RETURN: %s:%d\n", __func__, __LINE__); \
        return __VA_ARGS__;                            \
        fputc('\n', stdout);                           \
    } while(false)

#define TRACE(n, ...)                      \
    do {                                   \
        INDENT;                            \
        if((n) < verbosity) {              \
            printf("TRACE: " __VA_ARGS__); \
            fputc('\n', stdout);           \
        }                                  \
    } while(false)

#else
#define ENTER
#define RETURN(...)         \
    do {                    \
        return __VA_ARGS__; \
    } while(false)
#define TRACE(n, ...)
#endif

#endif /* _TRACE_H_ */
