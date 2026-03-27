
#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <stdio.h>
#include <stdlib.h> // for abort() and exit()

#ifdef USE_ASSERTS
#define ASSERT(expr, ...)                                                                                 \
    do {                                                                                                  \
        if(!(expr)) {                                                                                     \
            fprintf(stderr, "%s: %s: %d: assertion failed: (%s): ", __FILE__, __func__, __LINE__, #expr); \
            fprintf(stderr, __VA_ARGS__);                                                                 \
            fputc('\n', stderr);                                                                          \
            abort();                                                                                      \
        }                                                                                                 \
    } while(0)
#else
#define ASSERT(expr, ...)
#endif

#define FATAL(...)                                                                  \
    do {                                                                            \
        fprintf(stderr, "fatal error: %s: %s: %d: ", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                               \
        fputc('\n', stderr);                                                        \
        exit(1);                                                                    \
    } while(0)

int get_errors(void);
int get_warnings(void);
void error(const char* mark, const char* file, int line, int col, const char* fmt, ...);
void warning(const char* mark, const char* file, int line, int col, const char* fmt, ...);

#endif /* _ERRORS_H_ */
