
#include "common.h"

symbol_t* create_symbol(string_t* tag) {

    symbol_t* sym = _ALLOC_TYPE(symbol_t);
    sym->tag = tag;

    sym->line = get_line_no();
    sym->col = get_col_no();
    sym->fname = get_file_name();

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

    int val = comp_string(node->tag, sym->tag);
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
            sym->fname->buf, sym->line,  sym->col, sym->tag->buf);
    }
}

symbol_t* find_symbol(symbol_t* root, string_t* tag) {

    int val = comp_string(root->tag, tag);
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
void dump_symbol_table(symbol_t* node) {

    if(node != NULL) {
        dump_symbol_table(node->left);
        dump_symbol_table(node->right);

        printf("%c%*stag: \"%s\"\n", (!dump_level)? '\n': '\b', dump_level*2, "", node->tag->buf);

        dump_level++;
        if(node->repl_text != NULL) {
            printf("%*srepl text: %s\n", dump_level*2, "", node->repl_text->buf);
        }
        else
            printf("%*srepl text: -blank-\n", dump_level*2, "");
        dump_level--;

        if(node->parms != NULL) {
            dump_level++;
            printf("%*sparms:\n", dump_level*2, "");
            dump_symbol_table(node->parms);
            dump_level--;
        }
    }
}

#ifdef MODULE_TESTING
// build string:
// clang -DMODULE_TESTING -g -o t symbols.c string.c alloc.c error.c fileio.c

// for errors
file_t* file_stack = NULL;

int main(void) {

    const char* strs[] = {"flarp", "slop", "zap", "snark", "december", "park", "snap", "goober",  NULL};

    symbol_t* table = create_symbol(create_string("gabzonga"));
    for(int i = 0; strs[i] != NULL; i++)
        insert_symbol(table, create_symbol(create_string(strs[i])));

    insert_symbol(table, create_symbol(create_string("park")));

    symbol_t* sym = find_symbol(table, create_string("park"));
    printf("found: %s\n", sym->tag->buf);

    sym = find_symbol(table, create_string(strs[2]));
    printf("found: %s = %s\n", strs[2], sym->tag->buf);

    sym = find_symbol(table, create_string(strs[5]));
    printf("found: %s = %s\n", strs[5], sym->tag->buf);

    dump_symbol_table(table);
    destroy_sym_table(table);
}

#endif


