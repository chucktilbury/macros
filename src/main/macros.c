
#include "common.h"

// the '.' has not been consumed
int process_directive(void) {

    ENTER;
    int changes = 0;

    switch(expect_directive()) {
        case IF_DIRECTIVE:
            changes = process_ifelse();
            break;
        case ELSE_DIRECTIVE:
            error("misplaced else clause");
            break;
        case DEFINE_DIRECTIVE:
            changes = process_define();
            break;
        case IMPORT_DIRECTIVE:
            changes = process_import();
            break;
        case NOT_A_DIRECTIVE:
        default:
            EMITC('.');
            consume_char();
            break;
    }

    RETURN(changes);
}

int process_reference(void) {

    ENTER;
    int count = 0;

    string_t* str = expect_ref();
    if(str != NULL) {
        EMITS(str);
        count++;
    }
    else {
        int ch = get_char();
        TRACE("char on entry: %s", prnch(ch));
        consume_char();

        string_t* name = expect_name();
        TRACE("symbol not found: @%s", name->buf);

        EMITC('@');
        EMITS(name);
    }

    RETURN(count);
}

int process_file(void) {

    ENTER;
    int changes = 0;

    while(true) {
        int ch = get_char();
        switch(ch) {
            case '/':
                consume_char();
                ch = get_char();
                TRACE("char here: %c", ch);
                if(ch == '/') {
                    consume_char();
                    changes += consume_single_line_comment();
                }
                else if(ch == '*') {
                    consume_char();
                    changes += consume_multi_line_comment();
                }
                else {
                    //printf("\n>>> here\n");
                    // append_string_char(master, '/');
                    EMITC('/');
                }
                break;
            case '.':
                changes += process_directive();
                break;
            case '@':
                changes += process_reference();
                break;
            case EOF:
                if(IS_FILE) {
                    TRACE("end of file");
                    close_file();
                    consume_char();
                }
                else {
                    TRACE("end of buffer");
                    RETURN(changes);
                }
                break;
            case EOI:
                TRACE("end of input");
                RETURN(changes);
            default:
                //printf("-0x%02X\n", ch);
                EMITC(ch);
                consume_char();
                break;
        }
    }
    // never happens
    RETURN(changes);
}

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("macros", "simple macro processor", "0.1");
    add_cmdline('v', "verbosity", "verbosity", "Print more information", "1", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('I', NULL, "path", "Add to the import path", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('D', NULL, "define", "Add a macro to the text", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('o', "ofile", "ofile", "Specify the output file name", "output.txt", NULL, CMD_STR | CMD_ARGS);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);
#ifdef USE_TRACE
    init_trace();
#endif
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    ENTER;
    string_t* fname = get_cmd_opt("files");
    const char* ofile = raw_string(get_cmd_opt("ofile"));

    open_file(fname);
    set_output_buffer(ofile);
    process_file();

#ifdef USE_TRACE
    int passes = 0;
#endif

    do {
#ifdef USE_TRACE
        dump_char_buffer("output buffer", get_output_buffer());
        passes++;
        TRACE("\n\n------------------reprocess pass %d -------------------\n", passes);
#endif
        write_char_buffer(get_output_buffer());
        open_file(create_string(ofile));
        set_output_buffer(ofile);
    } while(process_file());

#ifdef USE_TRACE
    TRACE("number of passes: %d", passes);
    if(verbosity >= DEFAULT_TRACE + 10) {
        dump_char_buffer("output buffer", get_output_buffer());
        dump_symbol_table();
    }
#endif

    if(verbosity) {
        printf("%d errors, %d warnings\n", get_errors(), get_warnings());
    }

    RETURN(0);
}
