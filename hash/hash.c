#include <stdio.h>
#include <stdlib.h>



int main() {
    size_t i;
    FILE* file;
    uint8_t buffer[64];
    int c;

    file = fopen("test", "r");
    if (!file) {
        fprintf(stderr, "can't open file\n");
        return 1;
    }

    /* read 512 bits of the file, or until EOF */
    for (i = 0; i < 64; i++) {
        if ((c = fgetc(file)) != EOF) {
            buffer[i] = c;
        }
        else {
            break;
        }
    }
    /* fill the rest of the 512 bit block if needed */
    if (i < 64) {

    for (; i < 64; i++) {
        

    
    
    fclose(file);

    return 0;
}
