
#include "common.h"

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
    string_t* str;
    int val;
} token_t;

typedef struct {
    token_t** buf;
    int cap;
    int len;
} token_list_t;

static token_list_t* stack = NULL;
static token_list_t* queue = NULL;

static inline int get_prec(token_t* tok) {

    return (tok->type == NOT)  ? 4 :
            (tok->type == AND) ? 1 :
            (tok->type == OR)  ? 0 :
            (tok->type == LT)  ? 3 :
            (tok->type == GT)  ? 3 :
            (tok->type == LTE) ? 3 :
            (tok->type == GTE) ? 3 :
            (tok->type == EQ)  ? 2 :
            (tok->type == NEQ) ? 2 :
                                 -1;
}

static inline const char* tok_to_name(token_t* tok) {

    return (tok->type == NAME)    ? "NAME" :
            (tok->type == NUMBER) ? "NUMBER" :
            (tok->type == OPAREN) ? "OPAREN" :
            (tok->type == CPAREN) ? "CPAREN" :
            (tok->type == NOT)    ? "NOT" :
            (tok->type == AND)    ? "AND" :
            (tok->type == OR)     ? "OR" :
            (tok->type == LT)     ? "LT" :
            (tok->type == GT)     ? "GT" :
            (tok->type == LTE)    ? "LTE" :
            (tok->type == GTE)    ? "GTE" :
            (tok->type == EQ)     ? "EQ" :
            (tok->type == NEQ)    ? "NEQ" :
            (tok->type == VAL)    ? "VAL" :
                                    "UNKNOWN";
}

#ifdef USE_TRACE
#define SHOW_TOK(s, t)                               \
    do {                                             \
        if((t) != NULL) {                            \
            if((t)->type == NAME | (t)->type == VAL) \
                TRACE(10, "%s:\t%s:\t%s\t%s",        \
                      (s), (t)->str->buf,            \
                      tok_to_name((t)),              \
                      (t)->val ? "TRUE" : "FALSE");  \
            else if((t)->type == NUMBER)             \
                TRACE(10, "%s:\t%s:\t%s\t%d",        \
                      (s), (t)->str->buf,            \
                      tok_to_name((t)),              \
                      (t)->val);                     \
            else                                     \
                TRACE(10, "%s:\t%s:\t%s",            \
                      (s), (t)->str->buf,            \
                      tok_to_name((t)));             \
        }                                            \
        else                                         \
            TRACE(10, "%s:\tNULL\n", (s));           \
    } while(false)
#endif

token_list_t* create_token_list(void) {

    token_list_t* ptr = _ALLOC_TYPE(token_list_t);
    ptr->len = 0;
    ptr->cap = 1 << 3;
    ptr->buf = _ALLOC_ARRAY(token_t*, ptr->cap);

    return ptr;
}

void destroy_token(token_t* ptr) {

    if(ptr != NULL) {
        _FREE(ptr->str);
        _FREE(ptr);
    }
}

void destroy_token_list(token_list_t* ptr) {

    if(ptr != NULL) {
        for(int i = 0; i < ptr->len; i++)
            destroy_token(ptr->buf[i]);
        _FREE(ptr->buf);
        _FREE(ptr);
    }
}

token_t* create_token(string_t* str, token_type_t type, int val) {

    token_t* tok = _ALLOC_TYPE(token_t);

    tok->str = copy_string(str); // could be NULL
    tok->type = type;
    tok->val = val;

    return tok;
}

void append_token(token_list_t* lst, token_t* tok) {

    if(lst->len + 1 > lst->cap) {
        lst->cap <<= 1;
        lst->buf = _REALLOC_ARRAY(lst->buf, token_t*, lst->cap);
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
        ptr = stack->buf[stack->len - 1];
        stack->len--;
    }

    SHOW_TOK("pop ", ptr);
    return ptr;
}

token_t* peek_token(void) {

    token_t* ptr = NULL;
    if(stack->len > 0)
        ptr = stack->buf[stack->len - 1];

    SHOW_TOK("peek", ptr);
    return ptr;
}

void queue_token(token_t* ptr) {

    SHOW_TOK("queu", ptr);
    append_token(queue, ptr);
}

void show_token_list(const char* str, token_list_t* lst) {

    TRACE(10, "---- >%s ----", str);
    TRACE(10, "cap: %d, len: %d", lst->cap, lst->len);
    for(int i = 0; i < lst->len; i++)
        SHOW_TOK("LIST", lst->buf[i]);
    TRACE(10, "---- <%s ----", str);
}

static string_t* is_a_number(string_t* s) {


    int idx;
    string_t* tmp;

    for(idx = 0; idx < s->len; idx++) {
        if(!isspace(s->buf[idx]))
            break;
    }

    if(isdigit(s->buf[idx])) {

        tmp = create_string(NULL);

        for(; idx < s->len; idx++) {
            if(isdigit(s->buf[idx]))
                append_string_char(tmp, s->buf[idx]);
            else
                break;
        }
        return tmp;
    }

    return NULL;
}

static token_t* scan_number(void) {
    ENTER;

    string_t* s = create_string(NULL);
    int ch = get_char();

    while(isdigit(ch)) {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
    }

    token_t* tok = create_token(s, NUMBER, atoi(s->buf));
    TRACE(10, "scanned: %s: %s: %d", tok->str->buf, tok_to_name(tok), tok->val);

    RETURN(tok);
}

static token_t* scan_name_oper(token_t* tok) {
    ENTER;

    string_t* tmp = upcase_string(tok->str);
    TRACE(10, "string = %s", tok->str->buf);

    if(!comp_string(tmp, "AND")) {
        tok->type = AND;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "OR")) {
        tok->type = OR;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "EQ")) {
        tok->type = EQ;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "NEQ")) {
        tok->type = NEQ;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "LT")) {
        tok->type = LT;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "GT")) {
        tok->type = GT;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "LTE")) {
        tok->type = LTE;
        tok->val = 0;
    }
    else if(!comp_string(tmp, "GTE")) {
        tok->type = GTE;
        tok->val = 0;
    }
    // else do nothing

    destroy_string(tmp);
    RETURN(tok);
}

static token_t* scan_name(void) {
    ENTER;

    string_t* s = create_string(NULL);
    int ch = get_char();

    if(isalpha(ch) || ch == '_') {
        while(isalnum(ch) || ch == '_') {
            append_string_char(s, ch);
            consume_char();
            ch = get_char();
        }
    }
    else
        consume_error("a name");

    token_t* tok = create_token(s, NAME, 0);
    if(find_symbol(sym_table, tok->str))
        tok->val = 1;
    scan_name_oper(tok);
    TRACE(10, "scanned: %s: %s: %d", tok->str->buf, tok_to_name(tok), tok->val);

    RETURN(tok);
}

static token_t* scan_subst(void) {
    ENTER;

    consume_char(); // consume the '@'
    token_t* tok = scan_name();

    if(tok->type == NAME) {
        symbol_t* sym = find_symbol(sym_table, tok->str);
        if(sym != NULL) {
            tok->val = 1;
            if(sym->repl_text != NULL) {
                string_t* tmp = is_a_number(sym->repl_text);
                if(tmp != NULL) {
                    tok->type = NUMBER;
                    tok->val = atoi(tmp->buf);
                    destroy_string(tmp);
                }
            }
        }
    }
    else
        consume_error("a name");

    TRACE(10, "scanned: %s: %s: %d", tok->str->buf, tok_to_name(tok), tok->val);

    RETURN(tok);
}

static token_t* scan_oper(void) {
    ENTER;

    int ch = get_char();
    string_t* s = create_string(NULL);
    token_t* tok = NULL;

    if(ch == '@')
        tok = scan_subst();
    else if(ch == '!') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '=') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, NEQ, 0);
        }
        else
            tok = create_token(s, NOT, 0);
    }
    else if(ch == '<') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '=') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, LTE, 0);
        }
        else
            tok = create_token(s, LT, 0);
    }
    else if(ch == '>') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '=') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, GTE, 0);
        }
        else
            tok = create_token(s, GT, 0);
    }
    else if(ch == '&') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '&') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, AND, 0);
        }
        else
            consume_error("a AND operator");
    }
    else if(ch == '|') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '|') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, OR, 0);
        }
        else
            consume_error("an OR operator");
    }
    else if(ch == '=') {
        append_string_char(s, ch);
        consume_char();
        ch = get_char();
        if(ch == '=') {
            append_string_char(s, ch);
            consume_char();
            tok = create_token(s, EQ, 0);
        }
        else
            consume_error("an EQ operator");
    }
    else if(ch == '(') {
        append_string_char(s, ch);
        consume_char();
        tok = create_token(s, OPAREN, 0);
    }
    else if(ch == ')') {
        append_string_char(s, ch);
        consume_char();
        tok = create_token(s, CPAREN, 0);
    }
    else
        consume_error("an operator");

    TRACE(10, "scanned: %s: %s: %d", tok->str->buf, tok_to_name(tok), tok->val);

    RETURN(tok);
}

static token_t* scan_token(void) {
    ENTER;

    token_t* tok = NULL;
    consume_space();
    test_end();

    int ch = get_char();
    if(isdigit(ch))
        tok = scan_number();
    else if(ispunct(ch))
        tok = scan_oper();
    else if(isalpha(ch) || ch == '_')
        tok = scan_name();

    RETURN(tok);
}

bool solve(void) {

    ENTER;
    stack->len = 0;
    token_t* tok;

    TRACE(10, "---- solve ----");
    for(int i = 0; i < queue->len; i++) {
        tok = queue->buf[i];
        if(tok == NULL)
            error("NULL queue in solver");

        switch(tok->type) {
            case NAME:
            case NUMBER:
            case VAL:
                TRACE(10, "OPERAND");
                push_token(tok);
                break;
            case NOT: {
                TRACE(10, "NOT");
                token_t* oper = pop_token();
                push_token(create_token(NULL, VAL, !oper->val));
            } break;
            case AND: {
                TRACE(10, "AND");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val && right->val)));
            } break;
            case OR: {
                TRACE(10, "OR");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val || right->val)));
            } break;
            case LT: {
                TRACE(10, "LT");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val < right->val)));
            } break;
            case GT: {
                TRACE(10, "GT");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val > right->val)));
            } break;
            case LTE: {
                TRACE(10, "LTE");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val <= right->val)));
            } break;
            case GTE: {
                TRACE(10, "GTE");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val >= right->val)));
            } break;
            case EQ: {
                TRACE(10, "EQ");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val == right->val)));
            } break;
            case NEQ: {
                TRACE(10, "NEQ");
                token_t* right = pop_token();
                token_t* left = pop_token();
                push_token(create_token(NULL, VAL, (left->val != right->val)));
            } break;
            default:
                TRACE(10, "unexpected token in solver: \"%s\"", tok->str->buf);
                exit(1);
        }
    }

    if(stack->len != 1) {
        error("solver error stack len != 1");
        exit(1);
    }

    tok = pop_token();
    if(tok == NULL) {
        error("solver error stack is empty");
        exit(1);
    }

    TRACE(10, "----- %s ------", tok->val ? "TRUE" : "FALSE");
    RETURN(tok->val);
}

bool expression(void) {

    ENTER;
    bool finished = false;
    int count = 0;
    token_t* tok;

    if(stack == NULL)
        stack = create_token_list();
    else
        stack->len = 0;

    if(queue == NULL)
        queue = create_token_list();
    else
        queue->len = 0;

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
                TRACE(10, "OPERAND");
                queue_token(tok);
                break;
            case OPAREN:
                TRACE(10, "OPAREN");
                push_token(tok);
                count++;
                break;
            case CPAREN: {
                TRACE(10, "CPAREN");
                count--;
                if(count == 0)
                    finished = true;

                token_t* tmp;
                while((NULL != (tmp = peek_token())) && tmp->type != OPAREN)
                    queue_token(pop_token());

                if((NULL == (tmp = peek_token())) || tmp->type != OPAREN) {
                    TRACE(10, "imbalanced ')'");
                    exit(1);
                }
                else {
                    pop_token();
                    // if(stack->len == 0)
                    //     return true; // empty expression
                }
            } break;
            default: {
                TRACE(10, "OPERATOR: %s", tok_to_name(tok));
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
            } break;
        }
    }
    TRACE(10, "FINISH");
    // this should never happen because the enclosed "()"
    while(NULL != (tok = peek_token())) {
        // TRACE(10,"here (3)");
        queue_token(tok);
        pop_token();
    }

    // make the trace messages flow properly
    bool val = solve();
    RETURN(val);
}

#if 0
    ENTER;
    consume_space();

    string_t* s = create_string(NULL);
    token_t* tok;
    bool finished = false;
    int ch, state = 0;

    while(!finished) {
        test_end();
        ch = get_char();
        switch(state) {
            case 0:
                TRACE(30, "scanner state: %d", state);
                if(isdigit(ch)) {
                    TRACE(10, "NUMBER");
                    state = 100;
                }
                else if(isalpha(ch) || ch == '_' || ch == '@') {
                    TRACE(10, "NAME");
                    state = 200;
                }
                else if(ispunct(ch)) {
                    TRACE(10, "OPERATOR");
                    state = 300;
                }
                else
                    error("unknown character value in an expression: %c (0x%02X)", ch, ch);
                break;

            case 100:
                TRACE(30, "scanner state: %d", state);
                if(!isdigit(ch)) {
                    tok = create_token(s, NUMBER, atoi(s->buf));
                    state = 1000;
                }
                else {
                    append_string_char(s, ch);
                    consume_char();
                }
                break;

            case 200:
                TRACE(30, "scanner state: %d", state);
                if(ch != '@')
                    state = 210;
                else {
                    consume_char();
                    state = 240;
                }
                break;

            case 210:
                TRACE(30, "scanner state: %d", state);
                {
                    string_t* tmp = upcase_string(s);
                    TRACE(30, "state 210 string: %s", tmp->buf);
                    if(!comp_string(tmp, "AND")) {
                        tok = create_token(s, AND, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "OR")) {
                        tok = create_token(s, OR, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "NOT")) {
                        tok = create_token(s, NOT, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "EQ")) {
                        tok = create_token(s, EQ, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "NEQ")) {
                        tok = create_token(s, NEQ, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "GT")) {
                        tok = create_token(s, GT, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "LT")) {
                        tok = create_token(s, LT, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "GTE")) {
                        tok = create_token(s, GTE, 0);
                        state = 1000;
                    }
                    else if(!comp_string(tmp, "LTE")) {
                        tok = create_token(s, LTE, 0);
                        state = 1000;
                    }
                    else
                        state = 230;
                    destroy_string(tmp);
                }
                break;

            case 230:
                TRACE(30, "scanner state: %d", state);
                {
                    symbol_t* sym = find_symbol(sym_table, s);
                    if(sym == NULL)
                        tok = create_token(s, NAME, 0);
                    else if(sym->repl_text != NULL) {
                        string_t* tmp = is_a_number(sym->repl_text);
                        if(tmp != NULL) {
                            tok = create_token(s, NUMBER, atoi(tmp->buf));
                            destroy_string(tmp);
                        }
                        else {
                            // it was defined so it's true
                            tok = create_token(s, NAME, 1);
                        }
                    }
                    else {
                        // it was defined so it's true
                        tok = create_token(s, NAME, 1);
                    }
                    state = 1000;
                }
                break;

            case 240:
                TRACE(30, "scanner state: %d", state);
                ch = get_char();
                if(isalnum(ch)) {
                    append_string_char(s, ch);
                    consume_char();
                }
                else
                    state = 230;
                break;

            case 300:
                TRACE(30, "scanner state: %d", state);
                if(ch == '(') {
                    append_string_char(s, ch);
                    tok = create_token(s, OPAREN, 0);
                    consume_char();
                    state = 1000;
                }
                else if(ch == ')') {
                    append_string_char(s, ch);
                    tok = create_token(s, CPAREN, 0);
                    consume_char();
                    state = 1000;
                }
                else if(ch == '<') {
                    append_string_char(s, ch);
                    consume_char();
                    ch = get_char();
                    if(ch == '=') {
                        append_string_char(s, ch);
                        consume_char();
                        tok = create_token(s, LTE, 0);
                    }
                    else
                        tok = create_token(s, LT, 0);
                    state = 1000;
                }
                else if(ch == '>') {
                    append_string_char(s, ch);
                    consume_char();
                    ch = get_char();
                    if(ch == '=') {
                        append_string_char(s, ch);
                        consume_char();
                        tok = create_token(s, GTE, 0);
                    }
                    else
                        tok = create_token(s, GT, 0);
                    state = 1000;
                }
                else if(ch == '!') {
                    append_string_char(s, ch);
                    consume_char();
                    ch = get_char();
                    if(ch == '=') {
                        append_string_char(s, ch);
                        consume_char();
                        tok = create_token(s, NEQ, 0);
                    }
                    else
                        tok = create_token(s, NOT, 0);
                    state = 1000;
                }
                else if(ch == '=') {
                    append_string_char(s, ch);
                    consume_char();
                    ch = get_char();
                    if(ch == '=') {
                        append_string_char(s, ch);
                        consume_char();
                        tok = create_token(s, EQ, 0);
                    }
                    else
                        error("unknown operator \"=%c\" in scan_token()", ch);
                    state = 1000;
                }
                break;

            case 1000:
                TRACE(30, "scanner state: %d", state);
                finished = true;
                break;

            default:
                error("impossible state in expression token scanner");
        }
    }

    TRACE(10, "scanned: %s = %s", tok->str->buf, tok_to_name(tok));
    RETURN(tok);
#endif
