#include "common.h"
#include "symbols.h"
#include "process.h"
#include "ifelse.h"
#include "define.h"
#include "reference.h"


void cmdline(int argc, char** argv, char** env) {

    init_cmdline("macros", "simple macro processor", "0.1");
    add_cmdline('o', "ofile", "ofile", "Specify the output file name", NULL, NULL, CMD_STR | CMD_ARGS);
    add_cmdline('I', NULL, "path", "Add to the import path", NULL, NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('D', NULL, "define", "Add a macro to the text", "", NULL, CMD_STR | CMD_ARGS | CMD_LIST);
    add_cmdline('v', "verbosity", "verbosity", "Print more information", "1", NULL, CMD_NUM | CMD_ARGS);
    add_cmdline('h', "help", NULL, "Print this helpful information", NULL, cmdline_help, CMD_NONE);
    add_cmdline('V', "version", NULL, "Show the program version", NULL, cmdline_vers, CMD_NONE);
    add_cmdline(0, NULL, NULL, NULL, NULL, NULL, CMD_DIV);
    add_cmdline(0, NULL, "filename", "File name to input", NULL, NULL, CMD_REQD | CMD_ANON);

    parse_cmdline(argc, argv, env);

#ifdef USE_TRACE
    init_trace();
#endif

    int mark = 0;
    string_t* str = iterate_cmd_opt("path", &mark);
    while(str != NULL) {
        TRACE("add search dir: %s", str->buffer);
        add_dir_to_search(str);
        str = iterate_cmd_opt("path", &mark);
    }

    mark = 0;
    str = iterate_cmd_opt("define", &mark);
    while(str != NULL) {
        TRACE("add define: %s", str->buffer);
        add_define(str);
        str = iterate_cmd_opt("define", &mark);
    }

    setup_env();
}

int main(int argc, char** argv, char** env) {

    cmdline(argc, argv, env);

    ENTER;
    string_t* fname = get_cmd_opt("filename");

    load_input_buffer(fname);
    create_output_buffer();
    process_input();

    fname = get_cmd_opt("ofile");
    save_output_buffer(fname);

#ifdef USE_TRACE
    if(verbosity >= DEFAULT_TRACE - 10) {
        dump_output_buffer();
        dump_symbol_table();
    }
#endif

    if(verbosity && fname != NULL) {
        printf("%d errors, %d warnings\n", get_errors(), get_warnings());
    }

    RETURN(0);
}
