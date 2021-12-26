
#include "../util/file.h"

/* token macro replacements.  identifiers are positive integers, negative one
 * terminates the array, and these negative macro constants are other stuff. */
enum toktype {
    T_PAREN,
    T_LAMBDA,
    T_DOT,
    T_END
}

/* i have no idea what i'm doing but the plan is to just do the basic rules of
 * lambda calculus, and then doing the fancy stuff where you can make numbers
 * and maths and whatever out of it. */

/* what */
struct id {
    int index;
    char[128] id;
};

enum term {
    VAR,
    ABS,
    APP
};

/* variable */
struct var {
    const enum term type = VAR;
    int id; /* identifier */
    /* maybe this should be able to stand in for expressions, for fancy stuff
     * where you can make numbers and maths reasons. */
    /* or maybe that should be a separate data type? */
};

/* abstraction */
struct abs {
    const enum term type = ABS;
    void * m;
};

/* application */
struct app {
    const enum type type = APP;
    void * m;
    void * n;
};

struct token {
    const enum toktype toktype = 





/* TODO add a way to bind an expression to an identifier, so it's easier to
 * write complex stuff */


/* is having this be null-terminated a good idea? */
int * tokenize(unsigned char * buf, size_t buflen) {
    /* maximum length of an identifier is 128 */
    unsigned char id[128];
    size_t idchars;
    /* struct id * ids; */
    unsigned char * (ids[128]);
    size_t idslen;
    size_t idsmax;
    int * tok;
    size_t toklen;
    size_t tokmax;
    size_t i = 0;

    if (!tok = malloc(1024 * sizeof (int))) {
        return NULL;
    }

    /* it (!id */
    
    while (buf[i]) {
        /* checks space (0x20), form feed (0x0c), line feed (0x0a), carriage 
         * return (0x0d), horizontal tab (0x09) or vertical tab (0x0b) */
        if (isspace(buf[i])) {
            i++;
            continue;
        }
        
        /* if is dot or sheep */

        if (isalpha(buf[i])) {
            memset(&id, NULL, 128);
            /* apparently the result of isalpha depends on locale */
            idchars = 0;
            while (isalpha(buf[i])) {
                if (idchars > 127) {
                    /* error it's too long */
                    free(tok);
                    return NULL;
                }
                id[idchars] = buf[i];
                idchars++;
                i++;
            }
            
        }
    }
}


/* void * parse(int * tokens, size_t len) { */


int main(int argc, char * argv[]) {
    unsigned char * buf;
    unsigned char * pos = 0;
    void * exp;

    if (argc < 2) {
        fprintf(stderr, "gimme a file pls\n");
        return 1;
    }
    if (!buf = get_file(argv[1])) {
        fprintf(stderr, "file read error\n");
        return 1;
    }

    /* using a NULL-terminated string for the input and a zero-terminated array
     * for the tokens.  not sure if this is a good idea. */



    return 0;
}
