%{

#include "parser.h"
#include "fileio.h"
#include "common.h"

int depth;
string_t* strbuf = NULL;
void yyerror(const char* s);
int yycolno = 1;
int prev_lineno;

#define STRACE(...) TRACE("scanner: " __VA_ARGS__)

#define MAX_INCL 16
#define YY_USER_ACTION update_numbers();
#define SCANNER_STACK_SIZE 1024

static int scanner_stack[SCANNER_STACK_SIZE];
static int scanner_tos = 0;
static inline void push_scanner_state(int state) {
    ENTER;
    STRACE("push state: %d", state);
    if(scanner_tos > SCANNER_STACK_SIZE)
        FATAL("scanner stack over run");
    else {
        scanner_stack[scanner_tos] = state;
        scanner_tos++;
        BEGIN(state);
    }
    RETURN();
}

static inline int peek_scanner_state(void) {
    ENTER;
    if(scanner_tos >= 0 && scanner_tos < SCANNER_STACK_SIZE) {
        int state = scanner_stack[scanner_tos-1];
        STRACE("peek state: %d", state);
        RETURN(state);
    }
    else
        FATAL("scanner stack under run");
    RETURN(-1);
}

static inline void pop_scanner_state(void) {
    ENTER;
    STRACE("old state: %d", scanner_stack[scanner_tos-1]);
    if(scanner_tos == 0)
        FATAL("scanner stack under run");
    else {
        scanner_tos--;
        int state = scanner_stack[scanner_tos-1];
        STRACE("pop state: %d", state);
        BEGIN(state);
    }
    RETURN();
}


%}

%option yylineno
%option noinput
%option nounput
%option noyywrap
    // for cmake...
    //%option header-file="scanner.h"
    //%option outfile="scanner.c"

%x LCOMMENT BCOMMENT QSTR
%x IMP DEF EXP IFEL PARAM DBODY

%%

".import"|".include" {
    STRACE("import");
    push_scanner_state(IMP);
    return IMPORT;
}

".define" {
    STRACE("define");
    push_scanner_state(DEF);
    return DEFINE;
}

".if" {
    STRACE("if");
    push_scanner_state(IFEL);
    return IF;
}
".else"                 { return ELSE;}

"and"|"&&"              { return AND; }
"or"|"||"               { return OR; }
"not"|"!"               { return NOT; }

<IFEL>{
    "(" {
        push_scanner_state(EXP);
        return '(';
    }
    ")" {
        return ')';
    }
}

<DEF>"(" {
    STRACE("DEF:PARAM");
    push_scanner_state(PARAM);
    return '(';
}

<PARAM>")" {
    STRACE("PARAM:DEF");
    pop_scanner_state(); // from PARAM to DEF
    return ')';
}

<DEF>"{" {
    STRACE("DEF:DBODY");
    push_scanner_state(DBODY);
    if(strbuf == NULL)
        strbuf = create_string(NULL);
    else
        clear_string(strbuf);
    return '{';
}

<DBODY>"}" {
    STRACE("DBODY:???");
    pop_scanner_state(); // from DBODY to DEF
    pop_scanner_state(); // from DEF to what it was before def
    yylval.strg = strbuf;
    return '}';
}

<PARAM>[A-Za-z_][A-Za-z0-9_]* {
    STRACE("---PARAM:%s", yytext);
    yylval.strg = create_string(yytext);
    return NAME;
}

<PARAM>"," {
    return ',';
}

<DEF>[A-Za-z_][A-Za-z0-9_]* {
    STRACE("DEF:name: %s", yytext);
    yylval.strg = create_string(yytext);
    return NAME;
}

<DBODY>".if" {
    append_string_char(strbuf, yytext[0]);
}

<IMP>\" {
    if(strbuf == NULL)
        strbuf = create_string(NULL);
    else
        clear_string(strbuf);
    push_scanner_state(QSTR);
}

<QSTR>\n {
    //error("scanner", "%d: unexpected end of line in literal string\n", yylineno);
    fprintf(stderr, "scanner error: %d: unexpected end of line in literal string\n", yylineno);
    push_scanner_state(INITIAL);
}

<QSTR>\"[ \t]*\\[ \t]*\n[ \t]*\" { /* line ignore continuation */ }

<QSTR>[^\\\n\"]+ { append_string(strbuf, yytext); }

<QSTR>\" {
    yylval.strg = copy_string(strbuf);
    pop_scanner_state();
    pop_scanner_state();
    return STRG_LITERAL;
}

"//" {
    STRACE("push LCOMMENT %d:%d", get_line_no(), get_col_no());
    push_scanner_state(LCOMMENT);
}
<LCOMMENT>. {}
<LCOMMENT>[\n\r] {
    STRACE("pop LCOMMENT %d:%d", get_line_no(), get_col_no());
    pop_scanner_state();
}

"/*" {
    STRACE("push BCOMMENT %d:%d", get_line_no(), get_col_no());
    push_scanner_state(BCOMMENT);
}
<BCOMMENT>[\n\r] {}
<BCOMMENT>. {}
<BCOMMENT>"*/" {
    STRACE("pop BCOMMENT %d:%d", get_line_no(), get_col_no());
    pop_scanner_state();
}

 [ \t\r\n\v\f]|[^.] {

}

<<EOF>> {
    // This code must be inlined because of the heavy use of macros.
    file_t* ptr = get_file_buffer();
    if(ptr) {
        STRACE("end file: %s", raw_string(ptr->name));
        fclose(ptr->fp);
        destroy_string(ptr->name);
        yy_delete_buffer(ptr->buffer);
        pop_file_buffer();
        _FREE(ptr); // using GC, this is a NOOP
        ptr = get_file_buffer();
        if(ptr) {
            STRACE("switch to file: %s", raw_string(ptr->name));
            yy_switch_to_buffer(ptr->buffer);
        }
        else {
            STRACE("end of input");
            yyterminate();
        }
    }
}

%%

void init_scanner(void) {

    if(in_cmd_list("dump", "parser"))
        LOCAL_VERBOSITY(0);
    else
        LOCAL_VERBOSITY(19);

    ENTER;
    push_scanner_state(INITIAL);
    RETURN();
}