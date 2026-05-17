#ifndef __STRING_H_
#define __STRING_H_

typedef struct string_t {
    int len;
    int cap;
    char* buffer;
} string_t;

string_t* create_string(const char* str);
void destroy_string(string_t* s);
void append_string(string_t* s, int ch);
void append_string_str(string_t* s, const char* str);
void clear_string(string_t* s);
string_t* copy_string(string_t* str);

#endif /* __STRING_H_ */
