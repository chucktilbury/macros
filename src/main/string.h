#ifndef _STRING_H_
#define _STRING_H_

typedef struct {
    int len;
    int cap;
    char* buf;
} string_t;

string_t* create_string(const char* str);
void destroy_string(string_t* s);
void append_string(string_t* s, const char* str);
void append_string_char(string_t* s, int ch);
void clear_string(string_t* s);
int comp_string(string_t* left, string_t* right);
void strip_string(string_t* str, const char* pattern);
string_t* copy_string(string_t* str);

#endif /* _STRING_H_ */
