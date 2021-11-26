#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>



int main() {
    size_t i;
    FILE* file;
    uint8_t* buffer;
    int c;
    size_t buffer_len;
    long file_len;

    file = fopen("test", "r");
    if (!file) {
        fprintf(stderr, "can't open file\n");
        return 1;
    }

    /* TODO this only works on files, not streams */
    /* TODO check for errors */
    fseek(file, 0L, SEEK_END);
    file_len = ftell(file) + 1;
    /* TODO not sure if rewind or fseek is more appropriate (errors and eof) */
    rewind(file);
    /* message + the 9..72 bytes for padding and length */
    /* TODO double check this */
    buffer_len = file_len + 8 + (64 - ((file_len + 8) % 64));
    buffer = malloc(buffer_len);
    if (!buffer) {
        fprintf(stderr, "can't allocate memory\n");
        fclose(file);
        return 1;
    }
    /* put the stuff in buffer */
    /* TODO can the file be modified between the length check and here?  if so,
     * fix it */
    fread(buffer, 1, buffer_len, file);
    fclose(file);

    /* TEST */
    /*
    printf("%.*s\n", buffer_len, buffer);
    */

    /* padding with bits, a single 1 followed by however many 0 */
    

    
    
    return 0;
}
