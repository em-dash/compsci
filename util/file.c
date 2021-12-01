#include "file.h"

/* returns NULL-terminated string of the file contents */
/* memory allocated to the returned pointer is responsibility of the caller */
unsigned char * get_file(const char * filename) {
    unsigned char * buffer;
    size_t len;

    file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    /* TODO check for errors properly */
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    len = ftell(file) + 1;
    buffer = malloc(len);
    if (!buffer) {
        /* fprintf(stderr, "can't allocate memory\n"); */
        fclose(file);
        return NULL;
    }
    /* TODO not sure if rewind or fseek is more appropriate (errors and eof) */
    rewind(file);
    /* TODO can the file be modified between the length check and here?  if so,
     * fix it somehow i guess */
    fread(buffer, 1, len, file);
    buffer[len] = '\0';

    fclose(file);

    return buffer;
}
