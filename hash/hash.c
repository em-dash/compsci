#include <stdio.h>
#include <stdlib.h>



int main() {
    size_t i;
    FILE* file;
    uint8_t* buffer;
    int c;

    file = fopen("test", "r");
    if (!file) {
        fprintf(stderr, "can't open file\n");
        return 1;
    }
    buffer = malloc(64 * sizeof uint8_t);
    if (!buffer) {
        fprintf(stderr, "failed to allocate memory\n");
        return 1;
    }

    for (i = 0; i < 64; i++) {
        if ((c = fgetc(file)) != EOF) {
            buffer[i] = c;
        }
        else {
            break;
        }
    }

    
    
    fclose(file);
    free(buffer);

    return 0;
}
