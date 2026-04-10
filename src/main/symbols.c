
#include "common.h"

symbol_t* create_symbol(string_t* tag) {

    symbol_t* sym = _ALLOC_TYPE(symbol_t);
    sym->tag = tag;

    sym->line = LINE_NO;
    sym->col = COL_NO;
    sym->fname = create_string(FILE_NAME);

    return sym;
}

void destroy_sym_table(symbol_t* node) {

    if(node != NULL) {
        destroy_sym_table(node->left);
        destroy_sym_table(node->right);

        destroy_string(node->tag);
        destroy_string(node->repl_text);
        if(node->parms != NULL)
            destroy_sym_table(node->parms);
        _FREE(node);
    }
}

void insert_symbol(symbol_t* node, symbol_t* sym) {

    int val = comp_string_str(node->tag, sym->tag);
    if(0 > val) {
        if(node->right != NULL)
            insert_symbol(node->right, sym);
        else
            node->right = sym;
    }
    else if(0 < val) {
        if(node->left != NULL)
            insert_symbol(node->left, sym);
        else
            node->left = sym;
    }
    else {
        warning("ignoring re-definition of symbol: %s\n"
                "    previous definition was here: %s:%d:%d",
                sym->fname->buf, sym->line, sym->col, sym->tag->buf);
    }
}

symbol_t* find_symbol(symbol_t* root, string_t* tag) {

    if(root == NULL)
        return NULL;

    int val = comp_string_str(root->tag, tag);
    if(0 > val) {
        if(root->right != NULL)
            return find_symbol(root->right, tag);
        else
            return NULL;
    }
    else if(0 < val) {
        if(root->left != NULL)
            return find_symbol(root->left, tag);
        else
            return NULL;
    }
    else
        return root;
}

static int dump_level = 0;
static void _dump_symbol_table(symbol_t* node) {

    if(node != NULL) {
        _dump_symbol_table(node->left);
        _dump_symbol_table(node->right);

        printf("%c%*stag: \"%s\"\n", (!dump_level) ? '\n' : '\b', dump_level * 2, "", node->tag->buf);

        dump_level++;
        if(node->repl_text != NULL) {
            printf("%*srepl text: %s\n", dump_level * 2, "", node->repl_text->buf);
        }
        else
            printf("%*srepl text: -blank-\n", dump_level * 2, "");
        dump_level--;

        if(node->parms != NULL) {
            dump_level++;
            printf("%*sparms:\n", dump_level * 2, "");
            _dump_symbol_table(node->parms);
            dump_level--;
        }
    }
}

void dump_symbol_table(symbol_t* root) {

    printf("\n------------ symbols -------------\n");
    _dump_symbol_table(root);
    printf("\n----------------------------------\n");
}

