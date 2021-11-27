#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* left shift with rotate (32-bit unsigned integers) */
#define LROT32(n, x) (n << x) | (n >> 32 - x)

/* F, G, H, I auxiliary function macros */
/* TODO check this, esp operator precedence */
#define AF(x, y, z) x & y | ~x & z
#define AG(x, y, z) x & z | y & ~z
#define AH(x, y, z) x ^ y ^ z
#define AI(x, y, z) y ^ (x | ~z)


/* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define RF(a, b, c, d, xk, s, ti) \
    a = b + (LROT32(a + AF(b, c, d) + xk + ti), s)

/* a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define RG(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AG(b, c, d) + xk + ti), s)

/* a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define RH(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AH(b, c, d) + xk + ti), s)

/* a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define RI(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AI(b, c, d) + xk + ti), s)




int main() {
    FILE* file;
    uint8_t* buffer;
    uint32_t* buf_words;
    size_t buffer_len;
    long file_len;
    uint64_t len_append;
    uint64_t* len_append_addr;
    /* words A B C D in order */
    uint32_t mdbuf[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    uint32_t mdbuf_prev[4];
    uint32_t op_x[16];
    size_t i;

    /* TODO handle messages with a non-whole number of bytes */
    /* TODO handle messages too big to fit in memory all at once */
    /* TODO figure out portability to systems without uint8_t, and with other
     * integer and type differences.  idk. */

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

    /* padding and appending length */
    /* the actual message went up to buffer[file_len - 1] so we start padding at
     * buffer[file_len] */
    buffer[file_len] = (uint8_t) 128;
    /* high index = buffer_len - 1 - 8
     * low index = file_len + 1
     * count = high index - low index + 1
     * count = buffer_len - file_len - 9 */
    /* TODO check integer conversions in buffer + file_len + 1 */
    memset(buffer + file_len + 1, 0, buffer_len - file_len - 9);
    /* TODO properly handle "the unlikely event that b is greater than 2^64" */
    /* length of the original message in bits as a little-endian uint64_t */

    /* TODO detect endianness lmao */
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
    /* TODO check integer conversions */
    len_append_addr = (uint64_t*) (buffer + buffer_len - 8);
    *len_append_addr = file_len * 8;
#endif

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error "big endian not implemented (also macros don't work properly)"
    /* TODO big endian */
#endif

    /* process message */
    /* number of blocks = buffer_len / 64 */
    /* for each block i */
    /* TODO does this reevaluate buffer_len / 64 each time?  if so, fix.  (or
     * maybe just fix anyway.) */
    for (i = 0; i < buffer_len / 64; i++) {
        /* copy block i into X (length is 64 bytes = 16 * 32-bit words) */
        /* TODO check this makes sense */
        memcpy(op_x, buffer + i * 64, 64);
        /* make a copy of 4 word buffer (mdbuf to mdbuf_prev) */
        memcpy(mdbuf_prev, mdbuf, 16);


        /* intentionally allowing for overflows to wrap */
        /* ABCD is mdbuf[0] to mdbuf[3] */
        /* maybe they should just be individual variables instead, i'll figure
         * it out i guess */

        /* Round 1. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        [ABCD  0  7  1]
        [DABC  1 12  2]
        [CDAB  2 17  3]
        [BCDA  3 22  4]
        [ABCD  4  7  5]
        [DABC  5 12  6]
        [CDAB  6 17  7]
        [BCDA  7 22  8]
        [ABCD  8  7  9]
        [DABC  9 12 10]
        [CDAB 10 17 11]
        [BCDA 11 22 12]
        [ABCD 12  7 13]
        [DABC 13 12 14]
        [CDAB 14 17 15]
        [BCDA 15 22 16]

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        [ABCD  1  5 17]
        [DABC  6  9 18]
        [CDAB 11 14 19]
        [BCDA  0 20 20]
        [ABCD  5  5 21]
        [DABC 10  9 22]
        [CDAB 15 14 23]
        [BCDA  4 20 24]
        [ABCD  9  5 25]
        [DABC 14  9 26]
        [CDAB  3 14 27]
        [BCDA  8 20 28]
        [ABCD 13  5 29]
        [DABC  2  9 30]
        [CDAB  7 14 31]
        [BCDA 12 20 32]

        /* Round 3. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        [ABCD  5  4 33]
        [DABC  8 11 34]
        [CDAB 11 16 35]
        [BCDA 14 23 36]
        [ABCD  1  4 37]
        [DABC  4 11 38]
        [CDAB  7 16 39]
        [BCDA 10 23 40]
        [ABCD 13  4 41]
        [DABC  0 11 42]
        [CDAB  3 16 43]
        [BCDA  6 23 44]
        [ABCD  9  4 45]
        [DABC 12 11 46]
        [CDAB 15 16 47]
        [BCDA  2 23 48]

        /* Round 4. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        [ABCD  0  6 49]
        [DABC  7 10 50]
        [CDAB 14 15 51]
        [BCDA  5 21 52]
        [ABCD 12  6 53]
        [DABC  3 10 54]
        [CDAB 10 15 55]
        [BCDA  1 21 56]
        [ABCD  8  6 57]
        [DABC 15 10 58]
        [CDAB  6 15 59]
        [BCDA 13 21 60]
        [ABCD  4  6 61]
        [DABC 11 10 62]
        [CDAB  2 15 63]
        [BCDA  9 21 64]


    }
    
    return 0;
}
