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
        if(verbosity > 20) { \
            INDENT;                                     \
            printf("ENTER: %s:%d\n", __func__, __LINE__); \
            level++;                                    \
        } \
    } while(false)

#define RETURN(...)                                    \
    do {                                               \
        if(verbosity > 20) { \
            level--;                                       \
            INDENT;                                        \
            printf("RETURN(%s): %s:%d\n", #__VA_ARGS__, __func__, __LINE__); \
        } \
        return __VA_ARGS__;                            \
    } while(false)

#define TRACE(n, ...)                      \
    do {                                   \
        if((n) < verbosity) {              \
            INDENT;                        \
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
