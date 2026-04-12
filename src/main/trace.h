#ifndef _TRACE_H_
#define _TRACE_H_

#ifdef USE_TRACE

extern int level;
extern int verbosity;
#define DEFAULT_TRACE 50

#define INDENT                        \
    do {                              \
        printf("%*s", level * 2, ""); \
    } while(false)

#define ENTER                                               \
    do {                                                    \
        if(verbosity >= DEFAULT_TRACE) {                                \
            INDENT;                                         \
            printf("ENTER: %s():%d\n", __func__, __LINE__); \
            level++;                                        \
        }                                                   \
    } while(false)

#define RETURN(...)                                                            \
    do {                                                                       \
        if(verbosity >= DEFAULT_TRACE) {                                                   \
            level--;                                                           \
            INDENT;                                                            \
            printf("RETURN(%s): %s():%d\n", #__VA_ARGS__, __func__, __LINE__); \
        }                                                                      \
        return __VA_ARGS__;                                                    \
    } while(false)

#define TRACE(n, ...)                      \
    do {                                   \
        if(verbosity >= (n)) {              \
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

void init_trace(void);

#endif /* _TRACE_H_ */
