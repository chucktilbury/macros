// test shunting yard algo

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    NAME,
    NUMBER,
    OPAREN,
    CPAREN,
    NOT,
    AND,
    OR,
    LT,
    GT,
    LTE,
    GTE,
    EQ,
    NEQ,
    VAL,
} token_type_t;

typedef struct token_t {
    token_type_t type;
    //string_t* str;
    const char* str;
    int val;
} token_t;

typedef struct {
    token_t** buf;
    int cap;
    int len;
} token_list_t;

static token_list_t* stack = NULL;
static token_list_t* queue = NULL;

token_t* scan_token(void);

static inline int get_prec(token_t* tok) {

    return (tok->type == NOT)? 4:
        (tok->type == AND)? 1:
        (tok->type == OR)? 0:
        (tok->type == LT)? 3:
        (tok->type == GT)? 3:
        (tok->type == LTE)? 3:
        (tok->type == GTE)? 3:
        (tok->type == EQ)? 2:
        (tok->type == NEQ)? 2: -1;
}

// static inline int get_assoc(token_type_t type) {

//     // 1 = right assoc
//     return (type == NOT)? 1: 0;
// }

static inline const char* tok_to_name(token_type_t type) {

    return (type == NAME)? "NAME" :
        (type == NUMBER)? "NUMBER" :
        (type == OPAREN)? "OPAREN" :
        (type == CPAREN)? "CPAREN" :
        (type == NOT)? "NOT" :
        (type == AND)? "AND" :
        (type == OR)? "OR" :
        (type == LT)? "LT" :
        (type == GT)? "GT" :
        (type == LTE)? "LTE" :
        (type == GTE)? "GTE" :
        (type == EQ)? "EQ" :
        (type == NEQ)? "NEQ" :
        (type == VAL)? "VAL" : "UNKNOWN";
}

#define SHOW_TOK(s, t) \
    do { \
        if((t) != NULL) { \
            if((t)->type == NAME | (t)->type == VAL) \
                printf("%s:\t%s:\t%s\t%s\n", \
                    (s), (t)->str, \
                    tok_to_name((t)->type), \
                    (t)->val? "TRUE": "FALSE"); \
            else if((t)->type == NUMBER) \
                printf("%s:\t%s:\t%s\t%d\n", \
                    (s), (t)->str, \
                    tok_to_name((t)->type), \
                    (t)->val); \
            else \
                printf("%s:\t%s:\t%s\n", \
                    (s), (t)->str, \
                    tok_to_name((t)->type)); \
        } \
        else  \
            printf("%s:\tNULL\n", (s)); \
    } while(false)

token_list_t* create_token_list(void) {

    token_list_t* ptr = malloc(sizeof(token_list_t));
    ptr->len = 0;
    ptr->cap = 1 << 3;
    ptr->buf = (token_t**)malloc(ptr->cap * sizeof(token_t*));

    return ptr;
}

void destroy_token(token_t* ptr) {

    if(ptr != NULL) {
        free((void*)ptr->str);
        free(ptr);
    }
}

void destroy_token_list(token_list_t* ptr) {

    if(ptr != NULL) {
        for(int i = 0; i < ptr->len; i++)
            destroy_token(ptr->buf[i]);
        free(ptr);
    }
}

token_t* create_token(const char* str, token_type_t type, int val) {

    token_t* tok = malloc(sizeof(token_t));

    if(str != NULL)
        tok->str = strdup(str);
    else
        tok->str = NULL;

    tok->type = type;

    if(type == NAME || str == NULL)
        tok->val = val;
    else
        tok->val = atoi(str);

    return tok;
}

void append_token(token_list_t* lst, token_t* tok) {

    if(lst->len+1 > lst->cap) {
        lst->cap <<= 1;
        lst->buf = (token_t**)realloc(lst->buf, lst->cap*sizeof(token_t*));
    }

    lst->buf[lst->len] = tok;
    lst->len++;
}

void push_token(token_t* ptr) {

    SHOW_TOK("push", ptr);
    append_token(stack, ptr);
}

token_t* pop_token(void) {

    token_t* ptr = NULL;
    if(stack->len > 0) {
        ptr = stack->buf[stack->len-1];
        stack->len--;
    }

    SHOW_TOK("pop ", ptr);
    return ptr;
}

token_t* peek_token(void) {

    token_t* ptr = NULL;
    if(stack->len > 0)
        ptr = stack->buf[stack->len-1];

    SHOW_TOK("peek", ptr);
    return ptr;
}

void queue_token(token_t* ptr) {

    SHOW_TOK("queu", ptr);
    append_token(queue, ptr);
}

void show_token_list(const char* str, token_list_t* lst) {

    printf("---- >%s ----\n", str);
    printf("cap: %d, len: %d\n", lst->cap, lst->len);
    for(int i = 0; i < lst->len; i++)
        SHOW_TOK("LIST", lst->buf[i]);
    printf("---- <%s ----\n", str);
}

bool solve(void) {

    stack->len = 0;
    token_t* tok;

    printf("---- solve ----\n");
    for(int i = 0; i < queue->len; i++) {
        tok = queue->buf[i];
        if(tok == NULL) {
            printf("NULL queue in solver\n");
            exit(1);
        }

        switch(tok->type) {
            case NAME:
            case NUMBER:
            case VAL:
                printf("OPERAND\n");
                push_token(tok);
                break;
            case NOT: {
                    printf("NOT\n");
                    token_t* oper = pop_token();
                    push_token(create_token(NULL, VAL, !oper->val));
                }
                break;
            case AND: {
                    printf("AND\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val && right->val)));
                }
                break;
            case OR: {
                    printf("OR\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val || right->val)));
                }
                break;
            case LT: {
                    printf("LT\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val < right->val)));
                }
                break;
            case GT: {
                    printf("GT\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val > right->val)));
                }
                break;
            case LTE: {
                    printf("LTE\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val <= right->val)));
                }
                break;
            case GTE: {
                    printf("GTE\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val >= right->val)));
                }
                break;
            case EQ: {
                    printf("EQ\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val == right->val)));
                }
                break;
            case NEQ: {
                    printf("NEQ\n");
                    token_t* right = pop_token();
                    token_t* left = pop_token();
                    push_token(create_token(NULL, VAL, (left->val != right->val)));
                }
                break;
            default:
                printf("unexpected token in solver: \"%s\"\n", tok->str);
                exit(1);
        }
    }

    if(stack->len != 1) {
        printf("solver error stack len != 1\n");
        exit(1);
    }

    tok = pop_token();
    if(tok == NULL) {
        printf("solver error stack is empty\n");
        exit(1);
    }

    printf("---------------\n");
    return tok->val;
}

void expression(void) {

    bool finished = false;
    int count = 0;
    token_t* tok;

    while(!finished) {
        tok = scan_token();
        if(tok->str == NULL) {
            finished = true;
            continue;
        }

        switch(tok->type) {
            case NAME:
            case NUMBER:
            case VAL:
                printf("OPERAND\n");
                queue_token(tok);
                break;
            case OPAREN:
                printf("OPAREN\n");
                push_token(tok);
                count++;
                break;
            case CPAREN: {
                    printf("CPAREN\n");
                    count --;
                    if(count == 0)
                        finished = true;

                    token_t* tmp;
                    while((NULL != (tmp = peek_token())) && tmp->type != OPAREN)
                        queue_token(pop_token());

                    if((NULL == (tmp = peek_token())) || tmp->type != OPAREN) {
                        printf("imbalanced ')'\n");
                        exit(1);
                    }
                    else
                        pop_token();
                }
                break;
            default: {
                    printf("OPERATOR: %s\n", tok_to_name(tok->type));
                    while(true) {
                        token_t* tmp = peek_token();
                        if(tmp == NULL)
                            break;
                        else if(tmp->type == OPAREN)
                            break;
                        else if(get_prec(tok) <= get_prec(tmp))
                            queue_token(pop_token());
                        else
                            break;
                    }
                    push_token(tok);
                }
                break;

        }
    }
    printf("FINISH\n");
    // this should never happen because the enclosed "()"
    while(NULL != (tok = peek_token())) {
        printf("here (3)\n");
        queue_token(tok);
        pop_token();
    }
}

// ----------- test code ----------------

// (not (some and other) and (flarb or foob))
token_t stream1[] = {
    {OPAREN, "(", 0},
    {NOT, "not", 0},
    {OPAREN, "(", 0},
    {NAME, "some", 1},
    {AND, "and", 0},
    {NAME, "other", 0},
    {CPAREN, ")", 0},

    {AND, "and", 0},
    {OPAREN, "(", 0},
    {NAME, "flarb", 0},

    {OR, "or", 0},
    {NAME, "foob", 1},
    {CPAREN, ")", 0},

    {CPAREN, ")", 0},
    {-1, NULL, 0}
};

token_t stream[] = {
    {OPAREN, "(", 0},
    {OPAREN, "(", 0},
    {NUMBER, "10", 0},
    {LT, "<", 0},
    {NUMBER, "11", 0},
    {CPAREN, ")", 0},
    {AND, "and", 0},
    {NAME, "farb", 1},
    {CPAREN, ")", 0},
    {-1, NULL, 0}
};

token_t* scan_token(void) {

    static int idx = 0;

    if(stream[idx].str != NULL) {
        token_t* tok = create_token(stream[idx].str, stream[idx].type, stream[idx].val);
        idx++;
        return tok;
    }
    else
        return NULL;
}

int main(void) {

    // for(int i = 0; i < 4; i++) {
    //     token_t* tok = scan_token();
    //     push_token(tok);
    // }

    // for(int i = 0; i < 4; i++) {
    //     pop_token();
    // }

    // return(0);

    stack = create_token_list();
    queue = create_token_list();

    expression();
    printf("result: %s\n", solve()? "TRUE": "FALSE");

    // void* mark = NULL;
    // while(NULL != (tok = iterate_queue(&mark)))
    //     SHOW_TOK("QUEUE", tok);
    //     //printf("queue: %s:\t%s\n", tok->str, tok_to_name(tok->type));
    show_token_list("ending", queue);

    return 0;
}

