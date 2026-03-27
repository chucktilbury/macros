
#include "common.h"

#include <glob.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define DEF_EXT ".txt"

static string_list_t* common_env = NULL;
static char buffer[PATH_MAX]; // returning a pointer to this

/**
 * @brief Handle errors around realpath().
 *
 * @param str
 * @return const char*
 */
static inline const char* get_path(const char* str) {

    if(NULL == realpath(str, buffer))
        FATAL("cannot resolve path: \"%s\": %s", str, strerror(errno));

    return buffer;
}

/**
 * @brief Add a path defined in the environment to the internal finder path.
 *
 * @param str
 */
static void add_env(const char* str) {

    if(str != NULL) {
        char* tmp = getenv(str);
        if(tmp != NULL) {
            tmp = _COPY_STRING(tmp);
            // printf("%s = %s\n", str, tmp);
            if(tmp != NULL) {
                char* s;
                // char* save;
                char* f = ":";
                s = strtok(tmp, f);
                while(s != NULL) {
                    append_string_list(common_env, create_string(s));
                    s = strtok(NULL, f);
                }

                _FREE(tmp);
            }
        }
    }
}

/**
 * @brief Add a directory and all sub directories to the internal finder path.
 *
 * @param dname
 */
static void add_dirs(const char* dname) {

    char* tmp = NULL;
    struct stat s;

    tmp = (char*)get_path(dname);
    strcat(tmp, "/*");
    glob_t gstruct;
    glob(tmp, GLOB_NOSORT | GLOB_NOESCAPE, NULL, &gstruct);

    // printf("paths: %lu\n", gstruct.gl_pathc);
    for(size_t i = 0; i < gstruct.gl_pathc; i++) {
        // printf("%d. %s\n", i+1, gstruct.gl_pathv[i]);
        stat(gstruct.gl_pathv[i], &s);
        if(S_ISDIR(s.st_mode))
            append_ptr_list(common_env, create_string(gstruct.gl_pathv[i]));
    }
}

/**
 * @brief See if the file exists.
 *
 * @param fname
 * @return true
 * @return false
 */
static bool file_exists(const char* fname) {

    struct stat sb;
    return ((stat(fname, &sb) == 0));
}

/**
 * @brief Create the internal finder path environment.
 *
 */
static void setup_env(void) {

    common_env = create_string_list();

    add_env("PGEN_PATH");
    add_dirs("..");
    add_env("PATH");
}

/**
 * @brief Find a file. Returns the full path given just the name.
 *
 * @param fname
 * @return const char*
 */
static const char* find_file(const char* fname) {

    ENTER;

    char* found = NULL;

    // add the ".toy" on the end if it was not specified
    char* tmp_name = strrchr(fname, '.');
    if(NULL == tmp_name || strcmp(tmp_name, DEF_EXT)) {
        tmp_name = _ALLOC(PATH_MAX);
        strcpy(tmp_name, fname);
        strcat(tmp_name, DEF_EXT);
    }
    else
        tmp_name = _COPY_STRING(fname);

    TRACE(10, "searching for \"%s\"", tmp_name);

    if(common_env == NULL)
        setup_env();

    int mark = 0;
    string_t* s;

    while(NULL != (s = iterate_string_list(common_env, &mark))) {
        strncpy(buffer, raw_string(s), PATH_MAX);
        strcat(buffer, "/");
        strcat(buffer, tmp_name);

        TRACE(10, "try: %s", buffer);
        if(file_exists(buffer)) {
            TRACE(10, "found: %s", buffer);
            found = _COPY_STRING((buffer));
            break;
        }
    }

    _FREE(tmp_name);

    if(found == NULL)
        RETURN(fname);
    else
        RETURN(found);
}

void open_file(string_t* fname) {

    ASSERT(fname != NULL, "file name required");
    ENTER;

    FILE* fp;
    if(NULL == (fp = fopen(find_file(fname->buf), "r")))
        FATAL("cannot open input file: %s: %s", fname->buf, strerror(errno));

    TRACE(10, "opening file: %s", fname->buf);
    file_t* f = _ALLOC_TYPE(file_t);
    f->fp = fp;
    f->name = copy_string(fname);
    f->line = 1;
    f->col = 1;
    f->is_open = true;

    if(file_stack != NULL) {
        TRACE(10, "push file stack");
        f->next = file_stack;
    }
    file_stack = f;

    f->ch = consume_char();
    RETURN();
}

void close_file(void) {

    ASSERT(file_stack != NULL, "attempt to close a file but none have been opend");
    ASSERT(file_stack->is_open == true, "attempt to close a file that has already been closed");
    ENTER;

    file_t* f = file_stack;
    TRACE(10, "closing file: \"%s\"", f->name->buf);
    fclose(f->fp);
    f->is_open = false;

    // pop the stack but do not destroy the first node
    if(f->next != NULL) {
        TRACE(10, "pop file stack");
        file_stack = f->next;
        destroy_string(f->name);
        _FREE(f);
    }

    RETURN();
}

int get_char(void) {

    return file_stack->ch;
}

int consume_char(void) {

    ASSERT(file_stack != NULL, "attempt to read char but no file has been opend");

    if(file_stack->is_open) {
        if(file_stack->ch != EOF) {
            if(file_stack->ch == EOL) {
                file_stack->line++;
                file_stack->col = 1;
            }
            else
                file_stack->col++;

            file_stack->ch = fgetc(file_stack->fp);
        }
    }
    else
        file_stack->ch = EOI;

    return file_stack->ch;
}

int get_line_no(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->line;
}

int get_col_no(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->col;
}

string_t* get_file_name(void) {

    ASSERT(file_stack != NULL, "no file has been opend");
    return file_stack->name;
}
