
#include "common.h"


file_t* file_stack = NULL;
string_t* master = NULL;
symbol_t* sym_table = NULL;
int verbosity = 100;

// the "/*" has already been read
void consume_multi_line_comment(void) {

    ENTER;
    int ch;
    while(true) {
        ch = consume_char();
        if(ch == '*') {
            ch = consume_char();
            if(ch == '/') {
                consume_char();
                break;
            }
            else if(ch == EOF) {
                warning("unexpected end of file in comment");
                break;
            }
        }
        else if(ch == EOF) {
            warning("unexpected end of file in comment");
            break;
        }
    }
    RETURN();
}

// the "//" has already been read
void consume_single_line_comment(void) {

    ENTER;
    int ch;
    while(true) {
        ch = consume_char();
        if(ch == EOL) {
            consume_char();
            break;
        }
        else if(ch == EOF) {
            consume_char();
            warning("unexpected end of file in comment");
            break;
        }
    }
    RETURN();
}


// the '.' has not been consumed
void process_directive(void) {

    ENTER;
    string_t* tmp = create_string(NULL);
    int ch = get_char(); // should be the '.'

    // get the directive name
    do {
        append_string_char(tmp, ch);
        ch = consume_char();
    } while(!isspace(ch) && ch != EOF);

    if(!strcmp(tmp->buf, ".define"))
        process_define();
    else if(!strcmp(tmp->buf, ".import") || !strcmp(tmp->buf, ".include"))
        process_import();
    else if(!strcmp(tmp->buf, ".if"))
        process_ifelse();
    else {
        TRACE(0, "unmatched for directive: %s", tmp->buf);
        append_string(master, tmp->buf);
    }

    destroy_string(tmp);
    RETURN();
}

void process_file(void) {

    ENTER;
    while(true) {
        int ch = get_char();
        switch(ch) {
            case '/':
                ch = consume_char();
                if(ch == '/') {
                    consume_char();
                    consume_single_line_comment();
                }
                else if(ch == '*') {
                    consume_char();
                    consume_multi_line_comment();
                }
                else
                    append_string_char(master, '/');
                break;
            case '.':
                process_directive();
                break;
            case EOF:
                TRACE(0, "end of file");
                close_file();
                consume_char();
                break;
            case EOI:
                TRACE(0, "end of input");
                RETURN();
            default:
                append_string_char(master, ch);
                consume_char();
                break;
        }
    }
    // never happens
    RETURN();
}

int main(int argc, char** argv) {

    if(argc < 2) {
        error("use: %s filename", argv[0]);
        return 1;
    }
    else
        open_file(create_string(argv[1]));

    master = create_string(NULL);
    process_file();

    printf("----- master -----\n");
    printf("%s", master->buf);
    printf("----- symbols -----\n");
    dump_symbol_table(sym_table);

    return 0;
}
