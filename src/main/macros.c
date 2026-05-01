#include "common.h"
#include "symbols.h"
#include "misc.h"
#include "ifelse.h"
#include "define.h"
#include "reference.h"

/*
 * Process a reference in the root context.
 */
static void _process_reference(void) {
    ENTER;

    advance_char(); // the leading '@'
    string_t* name = scan_name();
    TRACE("name: %s", name->buffer);
    if(name != NULL) {
        symbol_t* sym = find_symbol(name);
        if(sym != NULL)
            process_reference();
        else {
            TRACE("not a reference");
            EMITC('@');
        }
    }
    destroy_string(name);

    RETURN();
}

/*
 * Scan the input for one of the objects that is acceptable outside of a
 * directive. If the character does not introduce an object then put it in the
 * output buffer.
 */
void process_file(void) {
    ENTER;

    bool finished = false;
    while(!finished) {
        int ch = crnt_char();
        switch(ch) {
            case '/': process_comment();   break;
            case '.': process_directive(); break;
            case '@': _process_reference(); break;
            case EOF:
                TRACE("end of file");
                pop_input_buffer();
                break;
            case EOI:
                TRACE("end of input");
                finished = true;
                break;
            default:
                // emit everything, including space
                EMITC(ch);
                advance_char();
                break;
        }
    }

    RETURN();
}

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("macros", "simple macro processor", "0.1");
    add_cmdline('i', "ifile", "ifile", "Specify the input file name", "input.txt", NULL, CMD_STR | CMD_ARGS);
    add_cmdline('o', "ofile", "ofile", "Specify the output file name", NULL, NULL, CMD_STR | CMD_ARGS);
    add_cmdline('I', NULL, "path", "Add to the import path", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('D', NULL, "define", "Add a macro to the text", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('v', "verbosity", "verbosity", "Print more information", "1", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    //    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    //    add_cmdline(0, NULL, "files", "File name(s) to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);
#ifdef USE_TRACE
    init_trace();
#endif
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    ENTER;
    string_t* fname = get_cmd_opt("ifile");
    // const char* ofile = raw_string(get_cmd_opt("ofile"));

    load_input_buffer(fname);
    create_output_buffer();
    process_file();

    save_output_buffer(get_cmd_opt("ofile"));
    // write_char_buffer(get_output_buffer());
    // #ifdef USE_TRACE
    //     int passes = 0;
    // #endif

    //     do {
    // #ifdef USE_TRACE
    //         dump_char_buffer("output buffer", get_output_buffer());
    //         passes++;
    //         TRACE("\n\n------------------reprocess pass %d -------------------\n", passes);
    // #endif
    //         write_char_buffer(get_output_buffer());
    //         open_file(create_string(ofile));
    //         set_output_buffer(ofile);
    //     } while(process_file());

#ifdef USE_TRACE
    //    TRACE("number of passes: %d", passes);
    if(verbosity >= DEFAULT_TRACE - 10) {
        dump_output_buffer();
        dump_symbol_table();
    }
#endif

    if(verbosity) {
        printf("%d errors, %d warnings\n", get_errors(), get_warnings());
    }

    RETURN(0);
}
