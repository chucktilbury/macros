%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "symbols.h"
#include "fileio.h"
#include "common.h"

int yylex(void);
void yyerror(const char*);
extern int yylineno;
extern FILE *yyin, *yyout;

int errors = 0;
int data_index = 0;
int data_count = 0;
unsigned long code_counter = 0;
pointer_list_t* data_ptrs = NULL;

#define PTRACE(...) TRACE("parser: " __VA_ARGS__)

%}

// support the typedefs in the %union.
%code requires {
#include "symbols.h"
#include "pointer_list.h"
}

// this goes at the bottom of the generated header file.
%code provides {
const char* token_to_str(int);
void run_parser(void);
extern int errors;
}

%union {
    string_t* strg;
    symbol_t* symbol;
};

%token IMPORT DEFINE IF ELSE AND OR NOT
%token <strg> STRG_LITERAL NAME NORMAL_TEXT REF_TEXT

%define parse.lac full
%define parse.error detailed
%locations
%verbose
    //%output "parser.c"
    //%defines

%left AND OR
%precedence UNARY_NOT

%%

module
    : module_item {
            PTRACE("FIRST module.module_item");
        }
    | module module_item {
            PTRACE("ADD module.module_item");
        }
    ;

module_item
    : define_directive {
        }
    | import_directive {
        }
    | if_directive {
        }
    | NORMAL_TEXT {
        }
    ;

parm_list
    : NAME {
        }
    | parm_list ',' NAME {
        }
    ;

define_directive
    : DEFINE NAME '(' parm_list ')' define_body {
        }
    | DEFINE NAME '(' ')' define_body {
        }
    | DEFINE NAME define_body {
        }
    ;

define_body
    : '{' define_body_list '}' {
        }
    | '{' '}' {
        }
    ;

define_body_element
    : NORMAL_TEXT {
            PTRACE("define_body_element.NORMAL_TEXT:\n %s\n", raw_string($1));
        }
    | if_directive {
        }
    ;

define_body_list
    : define_body_element {
        }
    | define_body_list define_body_element {
        }
    ;

import_directive
    : IMPORT STRG_LITERAL {
            PTRACE("import directive");
            open_file(raw_string($2));
        }
    ;

if_directive
    : if_clause else_clause_list {
        }
    | if_clause {
        }
    ;

if_clause
    : IF '(' expression ')' if_body {
        }
    ;

else_clause
    : ELSE '(' expression ')'  if_body {
        }
    ;

else_list
    : else_clause {
        }
    | else_list else_clause {
        }
    ;

final_else_clause
    : ELSE '(' ')' if_body {
        }
    | ELSE if_body {
        }
    ;

else_clause_list
    : else_list {
        }
    | else_list final_else_clause {
        }
    | final_else_clause {
        }
    ;

if_body
    : '{' if_body_list '}' {
        }
    ;

if_body_element
    : NORMAL_TEXT {
        }
    | define_directive {
        }
    | if_directive {
        }
    | import_directive {
        }
    ;

if_body_list
    : if_body_element {
        }
    | if_body_list if_body_element {
        }
    ;

expression
    : REF_TEXT {
        }
    | expression AND expression {
        }
    | expression OR expression {
        }
    | NOT expression %prec UNARY_NOT {
        }
    | '(' expression ')' {
        }
    ;
%%

extern char* yytext;
void yyerror(const char* s) {

    if(yytext[0] != '\0')
        fprintf(stderr, "%d:%d:%s: %s: %s\n", get_line_no(), get_col_no(), get_file_name(), s, yytext);
    else
        fprintf(stderr, "%d:%d:%s: %s\n", get_line_no(), get_col_no(), get_file_name(), s);
    errors++;
}

const char* token_to_str(int tok) {

    return yysymbol_name(YYTRANSLATE(tok));
}

void init_scanner(void);
void run_parser(void) {

    if(in_cmd_list("dump", "parser"))
        LOCAL_VERBOSITY(0);
    else
        LOCAL_VERBOSITY(9);

    TRACE_HEADER;

    //create_code_buffer();
    //create_data_buffer();
    create_symbol_table();

    init_scanner();
    const char* fname = raw_string(get_cmd_opt("files"));
    open_file(fname);

    yyparse();

    dump_sym_table();
    //dump_data_buffer();

}
