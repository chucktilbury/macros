
#include "common.h"


file_t* file_stack = NULL;
// string_t* master = NULL;
symbol_t* sym_table = NULL;
int verbosity = 0;

// the "/*" has already been read
void consume_multi_line_comment(void) {

    ENTER;
    int ch;
    int count = 0;
    while(true) {
        consume_char();
        count++;
        ch = get_char();
        if(ch == '*') {
            consume_char();
            count++;
            ch = get_char();
            if(ch == '/') {
                count++;
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
    TRACE(10, "count = %d", count);
    RETURN();
}

// the "//" has already been read
void consume_single_line_comment(void) {

    ENTER;
    int ch;
    int count = 0;
    while(true) {
        consume_char();
        count++;
        ch = get_char();
        if(ch == EOL) {
            consume_char();
            count++;
            break;
        }
        else if(ch == EOF) {
            consume_char();
            count++;
            warning("unexpected end of file in comment");
            break;
        }
    }
    TRACE(10, "count = %d", count);
    RETURN();
}


// the '.' has not been consumed
void process_directive(void) {

    ENTER;

    string_t* tmp = process_word();
    switch(process_directive_type(tmp)) {
        case IF_DIRECTIVE:
            process_ifelse();
            break;
        case ELSE_DIRECTIVE:
            error("misplaced else clause");
            break;
        case DEFINE_DIRECTIVE:
            process_define();
            break;
        case IMPORT_DIRECTIVE:
            process_import();
            break;
        case NOT_A_DIRECTIVE:
        default:
            // append_string_str(master, tmp);
            EMITS(tmp);
            break;
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
                consume_char();
                ch = get_char();
                if(ch == '/') {
                    consume_char();
                    consume_single_line_comment();
                }
                else if(ch == '*') {
                    consume_char();
                    consume_multi_line_comment();
                }
                else {
                    printf("\n>>> here\n");
                    // append_string_char(master, '/');
                    EMITC('/');
                }
                break;
            case '.':
                process_directive();
                break;
            case '@':
                process_subs();
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
                // printf("-0x%02X\n", ch);
                // append_string_char(master, ch);
                EMITC(ch);
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
    // add_cmdline('d', "dump", "dump", "Dump text as the parser is generated", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);
    verbosity = atoi(raw_string(get_cmd_opt("verbosity")));

    ENTER;
    string_t* fname = get_cmd_opt("files");

    open_file(fname);
    set_output_buffer(NULL);
    process_file();
    // process_substitutions(get_output_buffer());

    // printf("len: %d, cap: %d\n", master->len, master->cap);
    // printf("%s", master->buf);
    dump_char_buffer(get_output_buffer());
    dump_symbol_table(sym_table);

    RETURN(0);
}
