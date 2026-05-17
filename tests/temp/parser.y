%{
  //%code top {
/* bison -dtkv parser.y -o parser.c */
#include <stdio.h>
#include <stdlib.h>

extern int line_number;

// Initial states must be copied from the scanner.h file
#define INITIAL 0
#define COMMENT 1
#define STRING 2
#define DIRECTIVE 3
#define TEXT 4

int top_scanner_state(void);
void push_scanner_state(int state);
void pop_scanner_state(void);

void open_file(const char*);
const char* get_file_name(void);

  //}
%}

%code requires {
  #include "_string.h"
  int yylex(void);
  void yyerror(const char*);
}



%token DEFINE IF ELSE INCLUDE MESSAGE ERROR
%token EQU NEQU LT GT LTE GTE AND OR NOT
%token <str> QSTRG RAW_TEXT NAME BLOCK REFERENCE
%token <num> NUMBER

%union {
    string_t* str;
    int num;
}


%locations
%define parse.error detailed
%define parse.lac full

%left OR
%left AND
%left EQU NEQU
%left LT GT LTE GTE
%right NOT

%%

module
  : module_item
  | module module_item
  ;

module_item
  : define_clause
  | if_clause
  | reference
  | INCLUDE {
      push_scanner_state(DIRECTIVE);
    } QSTRG {
      open_file($3->buffer);
      destroy_string($3);
      pop_scanner_state();
    }
  | ERROR {
      push_scanner_state(DIRECTIVE);
    } QSTRG {
      fprintf(stderr, "ERROR: %s\n", $3->buffer);
      destroy_string($3);
      pop_scanner_state();
      exit(1);
    }
  | MESSAGE {
      push_scanner_state(DIRECTIVE);
    } QSTRG {
      fprintf(stdout, "MSG: %s\n", $3->buffer);
      destroy_string($3);
      pop_scanner_state();
    }
  ;

reference
  : REFERENCE ref_parm {
      printf("%d: reference name: %s\n", line_number, $1->buffer);
      pop_scanner_state();
    }
  ;

ref_parm_list_item
  : NAME {printf("name: %s\n", $1->buffer);}
  | reference
  | QSTRG
  | NUMBER {printf("number: %d\n", $1);}
  ;

ref_parm_list
  : ref_parm_list_item
  | ref_parm_list ',' ref_parm_list_item
  ;

ref_parm
  : %empty
  | '(' ')'
  | '(' ref_parm_list ')'
  ;


define
  : DEFINE {
      push_scanner_state(DIRECTIVE);
    }
  ;

define_clause
  : define NAME {
      printf("define name: %s\n", $2->buffer);
      pop_scanner_state();
    }
  | define NAME BLOCK {
      printf("define name: %s\n", $2->buffer);
      pop_scanner_state();
    }
  | define NAME '(' def_parm_list ')' BLOCK {
      pop_scanner_state();
    }
  ;

def_parm_list
  : NAME {
    }
  | def_parm_list ',' NAME {
    }
  ;

if_clause
  : IF {
      push_scanner_state(DIRECTIVE);
    } '(' expression ')' BLOCK else_clause {
    }
  ;

else
  : ELSE {
      push_scanner_state(DIRECTIVE);
    }
  ;

else_clause_item
  : else '(' expression ')' BLOCK {
      pop_scanner_state();
    }
  ;

else_clause_item_list
  : else_clause_item
  | else_clause_item_list else_clause_item
  ;

final_else_clause
  : else '(' ')' BLOCK {
      pop_scanner_state();
    }
  | else BLOCK {
      pop_scanner_state();
    }
  ;

else_clause
  : else_clause_item_list
  | else_clause_item_list final_else_clause
  | final_else_clause
  ;

expression
  : reference
  | NUMBER
  | NAME
  | expression EQU expression
  | expression NEQU expression
  | expression LT expression
  | expression GT expression
  | expression LTE expression
  | expression GTE expression
  | expression AND expression
  | expression OR expression
  | NOT expression
  | '(' expression ')'
  ;


%%

#include <stdio.h>

void yyerror(const char* msg) {
  fprintf(stderr, "%s:%d: %s\n", get_file_name(), line_number, msg);
}

