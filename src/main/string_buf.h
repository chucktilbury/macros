#ifndef _STRING_BUF_H_
#define _STRING_BUF_H_

typedef struct {
    int len;
    int cap;
    char* buf;
} string_t;

string_t* create_string(const char* str);
void destroy_string(string_t* s);
void append_string_str(string_t* s, string_t* str);
void append_string(string_t* s, const char* str);
void append_string_char(string_t* s, int ch);
void clear_string(string_t* s);
void strip_string(string_t* str, const char* pattern);
int comp_string_str(string_t* left, string_t* right);
int comp_string(string_t* buf1, const char* buf2);
string_t* upcase_string(string_t* s);
string_t* downcase_string(string_t* s);
string_t* copy_string(string_t* str);
const char* raw_string(string_t* str);

#endif /* _STRING_BUF_H_ */
