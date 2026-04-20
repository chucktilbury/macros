#include "common.h"

void cmdline(int argc, char** argv, char** env) {

    init_cmdline("macros", "simple macro processor", "0.1");
    add_cmdline('i', "ifile", "ifile", "Specify the input file name", "input.txt", NULL, CMD_STR | CMD_ARGS);
    add_cmdline('o', "ofile", "ofile", "Specify the output file name", "output.txt", NULL, CMD_STR | CMD_ARGS);
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
    const char* ofile = raw_string(get_cmd_opt("ofile"));

    open_file(fname);
    set_output_buffer(ofile);
    process_file();

    write_char_buffer(get_output_buffer());
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
        dump_char_buffer("output buffer", get_output_buffer());
        dump_symbol_table();
    }
#endif

    if(verbosity) {
        printf("%d errors, %d warnings\n", get_errors(), get_warnings());
    }

    RETURN(0);
}
