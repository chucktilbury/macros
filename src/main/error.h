#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef USE_ASSERT
#define ASSERT(e, ...)                                                               \
    do {                                                                             \
        if(!(e)) {                                                                   \
            fprintf(stderr, "assert failed: %s:%d: (%s): ", __func__, __LINE__, #e); \
            fprintf(stderr, __VA_ARGS__);                                            \
            fputc('\n', stderr);                                                     \
            abort();                                                                 \
        }                                                                            \
    } while(false)
#else
#define ASSERT(...)
#endif

#define FATAL(...)                                                   \
    do {                                                             \
        fprintf(stderr, "fatal error: %s:%d: ", __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                \
        fputc('\n', stderr);                                         \
        exit(1);                                                     \
    } while(false)

#define CHECK_EOF_ERROR(...)                              \
    do {                                                  \
        int ch = get_char();                              \
        if(ch == EOF || ch == EOI) {                      \
            consume_char();                               \
            error("unexpected end of file " __VA_ARGS__); \
            RETURN();                                     \
        }                                                 \
    } while(false);

void warning(const char* fmt, ...);
void error(const char* fmt, ...);
void consume_error(const char* where);
int expect_char(const char* str);
int get_errors(void);
int get_warnings(void);

#endif /* _ERROR_H_ */
