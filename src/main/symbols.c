
#include "common.h"

symbol_t* sym_table = NULL;

symbol_t* create_symbol(string_t* tag) {

    ENTER;
    TRACE("create_symbol: %s", tag->buf);
    symbol_t* sym = _ALLOC_TYPE(symbol_t);
    sym->tag = tag;
    //sym->arity = 0;

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
        // if(node->parms != NULL)
        //     _destroy_sym_table(node->parms);
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
                sym->tag->buf, sym->fname->buf, sym->line, sym->col);
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
    TRACE("search for: %s", tag->buf);

    symbol_t* sym = NULL;
    if(sym_table != NULL)
        sym = _find_symbol(sym_table, tag);

    TRACE("found = %s", sym? "TRUE": "FALSE");
    RETURN(sym);
}

// static void _dump_sym_parms(symbol_t* node) {

//     if(node != NULL) {
//         _dump_sym_parms(node->left);
//         _dump_sym_parms(node->right);
//         printf("\t\t%s\n", node->tag->buf);
//     }
// }

static void _dump_sym_table(symbol_t* node) {

    if(node != NULL) {
        _dump_sym_table(node->left);
        _dump_sym_table(node->right);

        printf("\ntag: \"%s\"\n", node->tag->buf);
        // printf("\tarity: %d\n", node->arity);
        // printf("\tparams:\n");
        // _dump_sym_parms(node->parms);
        if(node->repl_text != NULL) {
            printf("\trepl text: %s\n", node->repl_text->buf);
        }
        else
            printf("\trepl text: -blank-\n");
    }
}

void dump_symbol_table(void) {

    printf("\n------------ symbols -------------\n");
    _dump_sym_table(sym_table);
    printf("\n----------------------------------\n");
}
