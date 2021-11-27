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
        RF(A, B, C, D,  op_x[0],  7, 0xd76aa478);
        RF(D, A, B, C,  op_x[1], 12, 0xe8c7b756);
        RF(C, D, A, B,  op_x[2], 17, 0x242070db);
        RF(B, C, D, A,  op_x[3], 22, 0xc1bdceee);
        RF(A, B, C, D,  op_x[4],  7, 0xf57c0faf);
        RF(D, A, B, C,  op_x[5], 12, 0x4787c62a);
        RF(C, D, A, B,  op_x[6], 17, 0xa8304613);
        RF(B, C, D, A,  op_x[7], 22, 0xfd469501);
        RF(A, B, C, D,  op_x[8],  7, 0x698098d8);
        RF(D, A, B, C,  op_x[9], 12, 0x8b44f7af);
        RF(C, D, A, B, op_x[10], 17, 0xffff5bb1);
        RF(B, C, D, A, op_x[11], 22, 0x895cd7be);
        RF(A, B, C, D, op_x[12],  7, 0x6b901122);
        RF(D, A, B, C, op_x[13], 12, 0xfd987193);
        RF(C, D, A, B, op_x[14], 17, 0xa679438e);
        RF(B, C, D, A, op_x[15], 22, 0x49b40821);

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RF(A, B, C, D,   op_x[1],  5, 0xf61e2562);
        RF(D, A, B, C,   op_x[6],  9, 0xc040b340);
        RF(C, D, A, B,  op_x[11], 14, 0x265e5a51);
        RF(B, C, D, A,   op_x[0], 20, 0xe9b6c7aa);
        RF(A, B, C, D,   op_x[5],  5, 0xd62f105d);
        RF(D, A, B, C,  op_x[10],  9, 0x2441453);
        RF(C, D, A, B,  op_x[15], 14, 0xd8a1e681);
        RF(B, C, D, A,   op_x[4], 20, 0xe7d3fbc8);
        RF(A, B, C, D,   op_x[9],  5, 0x21e1cde6);
        RF(D, A, B, C,  op_x[14],  9, 0xc33707d6);
        RF(C, D, A, B,   op_x[3], 14, 0xf4d50d87);
        RF(B, C, D, A,   op_x[8], 20, 0x455a14ed);
        RF(A, B, C, D,  op_x[13],  5, 0xa9e3e905);
        RF(D, A, B, C,   op_x[2],  9, 0xfcefa3f8);
        RF(C, D, A, B,   op_x[7], 14, 0x676f02d9);
        RF(B, C, D, A,  op_x[12], 20, 0x8d2a4c8a);

        /* Round 3. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RF(A, B, C, D,   op_x[5],  4, 0xfffa3942);
        RF(D, A, B, C,   op_x[8], 11, 0x8771f681);
        RF(C, D, A, B,  op_x[11], 16, 0x6d9d6122);
        RF(B, C, D, A,  op_x[14], 23, 0xfde5380c);
        RF(A, B, C, D,   op_x[1],  4, 0xa4beea44);
        RF(D, A, B, C,   op_x[4], 11, 0x4bdecfa9);
        RF(C, D, A, B,   op_x[7], 16, 0xf6bb4b60);
        RF(B, C, D, A,  op_x[10], 23, 0xbebfbc70);
        RF(A, B, C, D,  op_x[13],  4, 0x289b7ec6);
        RF(D, A, B, C,   op_x[0], 11, 0xeaa127fa);
        RF(C, D, A, B,   op_x[3], 16, 0xd4ef3085);
        RF(B, C, D, A,   op_x[6], 23, 0x4881d05);
        RF(A, B, C, D,   op_x[9],  4, 0xd9d4d039);
        RF(D, A, B, C,  op_x[12], 11, 0xe6db99e5);
        RF(C, D, A, B,  op_x[15], 16, 0x1fa27cf8);
        RF(B, C, D, A,   op_x[2], 23, 0xc4ac5665);

        /* Round 4. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RF(A, B, C, D,   op_x[0],  6, 0xf4292244);
        RF(D, A, B, C,   op_x[7], 10, 0x432aff97);
        RF(C, D, A, B,  op_x[14], 15, 0xab9423a7);
        RF(B, C, D, A,   op_x[5], 21, 0xfc93a039);
        RF(A, B, C, D,  op_x[12],  6, 0x655b59c3);
        RF(D, A, B, C,   op_x[3], 10, 0x8f0ccc92);
        RF(C, D, A, B,  op_x[10], 15, 0xffeff47d);
        RF(B, C, D, A,   op_x[1], 21, 0x85845dd1);
        RF(A, B, C, D,   op_x[8],  6, 0x6fa87e4f);
        RF(D, A, B, C,  op_x[15], 10, 0xfe2ce6e0);
        RF(C, D, A, B,   op_x[6], 15, 0xa3014314);
        RF(B, C, D, A,  op_x[13], 21, 0x4e0811a1);
        RF(A, B, C, D,   op_x[4],  6, 0xf7537e82);
        RF(D, A, B, C,  op_x[11], 10, 0xbd3af235);
        RF(C, D, A, B,   op_x[2], 15, 0x2ad7d2bb);
        RF(B, C, D, A,   op_x[9], 21, 0xeb86d391);


    }
    
    return 0;
}
