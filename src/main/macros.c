
#include "common.h"


file_t* file_stack = NULL;
string_t* master = NULL;
symbol_t* sym_table = NULL;
int verbosity = 0;

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
        TRACE(10, "unmatched for directive: %s", tmp->buf);
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
                TRACE(10, "end of file");
                close_file();
                consume_char();
                break;
            case EOI:
                TRACE(10, "end of input");
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

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("macros", "simple macro processor", "0.1");
    add_cmdline('v', "verbosity", "verbosity", "Print more information", "0", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('p', "path", "path", "Add to the import path", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    //add_cmdline('d', "dump", "dump", "Dump text as the parser is generated", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);

    //INIT_TRACE(NULL);
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    verbosity = atoi(raw_string(get_cmd_opt("verbosity")));
    string_t* fname = get_cmd_opt("files");

    open_file(fname);

    // if(argc < 2) {
    //     error("use: %s filename", argv[0]);
    //     return 1;
    // }
    // else
    //     open_file(create_string(argv[1]));

    master = create_string(NULL);
    process_file();

    printf("----- master -----\n");
    printf("%s", master->buf);
    printf("----- symbols -----\n");
    dump_symbol_table(sym_table);

    return 0;
}
