
#include "common.h"
#include "symbols.h"
// #include "process.h"
#include "parms.h"

typedef struct symbol_context_t {
    symbol_t** sym;
    int cap;
    int len;
} symbol_context_t;

symbol_t* sym_table = NULL;
symbol_context_t* sym_context = NULL;

symbol_t* create_symbol(string_t* tag) {

    ENTER;
    TRACE("create_symbol: %s", tag->buffer);
    symbol_t* sym = _ALLOC_TYPE(symbol_t);
    sym->tag = tag;
    sym->arity = 0;
    sym->parms = create_parm_list();

    sym->line = LINE_NO;
    sym->col = COL_NO;
    sym->fname = create_string(FILE_NAME);

    RETURN(sym);
}

void _destroy_sym_table(symbol_t* node) {

    if(node != NULL) {
        _destroy_sym_table(node->left);
        _destroy_sym_table(node->right);

        destroy_string(node->tag);
        destroy_string(node->repl_text);
        destroy_parm_list(node->parms);
        _FREE(node);
    }
}

void destroy_sym_table(void) {

    ENTER;
    _destroy_sym_table(sym_table);
    RETURN();
}

void _insert_symbol(symbol_t* node, symbol_t* sym) {

    int val = comp_string_str(node->tag, sym->tag);
    if(0 > val) {
        if(node->right != NULL)
            _insert_symbol(node->right, sym);
        else
            node->right = sym;
    }
    else if(0 < val) {
        if(node->left != NULL)
            _insert_symbol(node->left, sym);
        else
            node->left = sym;
    }
    else {
        warning("ignoring re-definition of symbol: %s\n"
                "    previous definition was here: %s:%d:%d",
                sym->tag->buffer, sym->fname->buffer, sym->line, sym->col);
    }
}

symbol_t* insert_symbol(string_t* tag) {

    ENTER;

    symbol_t* sym = create_symbol(tag);
    if(sym_table == NULL)
        sym_table = sym;
    else
        _insert_symbol(sym_table, sym);

    RETURN(sym);
}

static symbol_t* _find_symbol(symbol_t* root, string_t* tag) {

    if(root == NULL)
        return NULL;

    int val = comp_string_str(root->tag, tag);
    if(0 > val) {
        if(root->right != NULL)
            return _find_symbol(root->right, tag);
        else
            return NULL;
    }
    else if(0 < val) {
        if(root->left != NULL)
            return _find_symbol(root->left, tag);
        else
            return NULL;
    }
    else
        return root;
}

symbol_t* find_symbol(string_t* tag) {

    ENTER;
    TRACE("search for: %s", tag->buffer);

    symbol_t* sym = NULL;
    if(sym_table != NULL)
        sym = _find_symbol(sym_table, tag);

    TRACE("found = %s", sym ? "TRUE" : "FALSE");
    RETURN(sym);
}

static void _dump_sym_table(symbol_t* node) {

    if(node != NULL) {
        _dump_sym_table(node->left);
        _dump_sym_table(node->right);

        printf("\ntag: \"%s\"\n", node->tag->buffer);
        printf("\tarity: %d\n", node->arity);
        printf("\tparams:\n");
        for(int i = 0; i < node->parms->len; i++)
            printf("\t\t%s\n", node->parms->lst[i]->name->buffer);
        if(node->repl_text != NULL) {
            printf("\trepl text: %s\n", node->repl_text->buffer);
        }
        else
            printf("\trepl text: -blank-\n");
    }
}

void dump_symbol_table(void) {

    print_legend("begin symbols");
    _dump_sym_table(sym_table);
    print_legend("end symbols");
}

#if 0
void push_symbol_context(symbol_t* sym) {
    ENTER;
    if(sym_context != NULL) {
        if(sym_context->len + 1 > sym_context->cap) {
            sym_context->cap <<= 1;
            sym_context->sym = _REALLOC_ARRAY(sym_context->sym, symbol_t*, sym_context->cap);
        }
        sym_context->sym[sym_context->len] = sym;
        sym_context->len++;
    }
    else {
        sym_context = _ALLOC_TYPE(symbol_context_t);
        sym_context->cap = 1 << 3;
        sym_context->len = 1;
        sym_context->sym = _ALLOC_ARRAY(symbol_t*, sym_context->cap);
        sym_context->sym[0] = sym;
    }
    TRACE("cap = %d, len = %d", sym_context->cap, sym_context->len);
    RETURN();
}

void pop_symbol_context(void) {
    ENTER;
    if(sym_context != NULL) {
        if(sym_context->len > 0) {
            sym_context->len--;
        }
        else
            error("symbol stack underrun");
    }
    RETURN();
}

symbol_t* peek_symbol_context(void) {
    ENTER;
    symbol_t* sym = NULL;
    if(sym_context != NULL) {
        if(sym_context->len-1 >= 0) {
            sym = sym_context->sym[sym_context->len-1];
            TRACE("sym name: \"%s\"", sym->tag->buffer);
        }
        TRACE("stack depth: %d", sym_context->len-1);
    }
#ifdef USE_TRACE
    else TRACE("null symbol context");
#endif

    RETURN(sym); // return NULL is stack is empty
}
#endif
