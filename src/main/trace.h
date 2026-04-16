#ifndef _TRACE_H_
#define _TRACE_H_

extern int verbosity;

#ifdef USE_TRACE

extern int level;
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

#define TRACE(...)                      \
    do {                                   \
        if(verbosity >= DEFAULT_TRACE) {              \
            INDENT;                        \
            printf("TRACE: " __VA_ARGS__); \
            fputc('\n', stdout);           \
        }                                  \
    } while(false)

#define TRACEX(n, ...)                      \
    do {                                   \
        if(verbosity >= (n)) {              \
            INDENT;                        \
            printf("TRACE: " __VA_ARGS__); \
            fputc('\n', stdout);           \
        }                                  \
    } while(false)

#define TRACEF \
    do { \
        if(verbosity >= DEFAULT_TRACE) { \
            INDENT; \
            printf("TRACE: %s: %d: %d\n", FILE_NAME, LINE_NO, COL_NO); \
        } \
    } while(false)

#else
#define ENTER
#define RETURN(...)         \
    do {                    \
        return __VA_ARGS__; \
    } while(false)
#define TRACE(...)
#define TRACEX(n, ...)
#define TRACEF
#endif

void init_trace(void);

#endif /* _TRACE_H_ */
