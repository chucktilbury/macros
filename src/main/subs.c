/*
 * Process a block of text and make a copy of it with the substitutions made.
 * This is performed as the very last step after the whole file has been
 * processed.
 *
 * 1) Scan the master block of text for strings like "@name".
 * 2) When one iis found, then look up "name" in the symbol table.
 * 3) If the name was defined, and it has replacement text, then replace the
 *      symbol with the replacement text. The replacement text may have symbols
 *      in it as well.
 * 4) If the name was not defined, then place the symbol in the output unchanged.
 * 5) If the symbol was defined but there is no replacement text, then delete
 *      the symbol.
 * 6) Re-scan the master block until there are no more replacements made.
 */

#include "common.h"


string_t* process_substitutions(string_t* str) {

    (void)str;
    return NULL;
}
