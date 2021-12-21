#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* "word" means 32-bit unsigned integer */

/* left shift with rotate (32-bit unsigned integers) */
/* x must be less than 32 */
/* apparently the compiler figured this out which is pretty neat.  not sure
 * whether to change it in some way. */
#define LROT32(n, x) ((n << x) | (n >> (32 - x)))

/* F, G, H, I auxiliary function macros */
#define AF(x, y, z) (x & y | ~x & z)
#define AG(x, y, z) (x & z | y & ~z)
#define AH(x, y, z) (x ^ y ^ z)
#define AI(x, y, z) (y ^ (x | ~z))

/* a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define RF(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AF(b, c, d) + xk + ti), s))

/* a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define RG(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AG(b, c, d) + xk + ti), s))

/* a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define RH(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AH(b, c, d) + xk + ti), s))

/* a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define RI(a, b, c, d, xk, s, ti) \
    a = b + (LROT32((a + AI(b, c, d) + xk + ti), s))




int main(int argc, char * argv[]) {
    int result;
    FILE * file;
    uint8_t * buffer;
    size_t buffer_len;
    long file_len;
    uint64_t * len_append_addr;
    /* words A B C D in order */
    uint32_t mdbuf[4] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
    /* words AA BB CC DD */
    uint32_t mdbuf_prev[4];
    /* uint32_t op_x[16]; */
    /* TODO would it help to make this uint32_t const * instead? */
    uint32_t * op_x;
    size_t i, j;
    uint8_t * out_bytes;
    /* TODO handle messages with a non-whole number of bytes */
    /* TODO handle messages too big to fit in memory all at once */
    /* TODO figure out portability to systems without uint8_t, and with other
     * integer and type differences.  especially: a lot of stuff here relies on
     * integers not having padding. */

    if (argc < 2) {
        fprintf(stderr, "gimme a file pls\n");
        return 1;
    }

    /* TODO filenames are multibyte strings but also it's complicated with unix
     * cause it just passes bytes verbaitum idk */
    /* the "b" file access flag does nothing on POSIX, but will stop windows
     * from doing special handling of line endings */
    file = fopen(argv[1], "rb");
    if (!file) {
        fprintf(stderr, "can't open file\n");
        return 2;
    }

    /* TODO this only works on file files, not streams */
    /* TODO check for errors */
    if (fseek(file, 0L, SEEK_END) != 0) {
        fprintf(stderr, "error reading file");
        fclose(file);
    }
    file_len = ftell(file);
    /* TODO not sure if rewind or fseek is more appropriate (errors and eof) */
    rewind(file);
    /* message + the bytes (9 to 72 inclusive) for padding and length */
    buffer_len = file_len + 8 + (64 - ((file_len + 8) % 64));
    buffer = malloc(buffer_len);
    if (!buffer) {
        fprintf(stderr, "can't allocate memory\n");
        fclose(file);
        return 1;
    }
    /* put the stuff in buffer */
    /* TODO can the file be modified between the length check and here?  if so,
     * fix it somehow i guess */
    fread(buffer, 1, file_len, file);
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
     * count = (buffer_len - 1 - 8) - (file_len + 1) + 1
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
    /* TODO does alignment matter with any of this??  seems likely malloc gives 
     * me a thing that's aligned in a way that would work for uint32_t anyway, 
     * but i'm not sure if that's always true */
    /* TODO does this reevaluate buffer_len / 64 each time?  if so, fix.  (or
     * maybe just fix anyway.) */
    for (i = 0; i < buffer_len / 64; i++) {
        /* copy block i into X (length is 64 bytes = 16 * 32-bit words) */
        /* TODO check this makes sense */
        /* TODO also this doesn't get changed in any way so we could just point 
         * to the original memory of the message (maybe with exceptions based on
         * loading the files and stuff idk) */
        /* TODO this is very depend on byte order */
        /* memcpy(op_x, buffer + i * 64, 64); */
        op_x = (uint32_t *) (buffer + i * 64);

        /* make a copy of 4 word buffer (mdbuf to mdbuf_prev) */
        /* this memcpy should be fine because it's to and from the same type */
        memcpy(mdbuf_prev, mdbuf, 16);

        /* intentionally allowing for overflows to wrap */
        /* ABCD is mdbuf[0] to mdbuf[3] */
        /* maybe they should just be individual variables instead, i'll figure
         * it out i guess */

        /* Round 1. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RF(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[0],  7, 0xd76aa478);
        RF(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[1], 12, 0xe8c7b756);
        RF(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[2], 17, 0x242070db);
        RF(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[3], 22, 0xc1bdceee);
        RF(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[4],  7, 0xf57c0faf);
        RF(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[5], 12, 0x4787c62a);
        RF(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[6], 17, 0xa8304613);
        RF(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[7], 22, 0xfd469501);
        RF(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[8],  7, 0x698098d8);
        RF(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[9], 12, 0x8b44f7af);
        RF(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1], op_x[10], 17, 0xffff5bb1);
        RF(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0], op_x[11], 22, 0x895cd7be);
        RF(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3], op_x[12],  7, 0x6b901122);
        RF(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2], op_x[13], 12, 0xfd987193);
        RF(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1], op_x[14], 17, 0xa679438e);
        RF(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0], op_x[15], 22, 0x49b40821);
        /* glad these fit in 80 characters */

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RG(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[1],  5, 0xf61e2562);
        RG(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[6],  9, 0xc040b340);
        RG(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[11], 14, 0x265e5a51);
        RG(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[0], 20, 0xe9b6c7aa);
        RG(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[5],  5, 0xd62f105d);
        RG(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[10],  9, 0x2441453);
        RG(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[15], 14, 0xd8a1e681);
        RG(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[4], 20, 0xe7d3fbc8);
        RG(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[9],  5, 0x21e1cde6);
        RG(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[14],  9, 0xc33707d6);
        RG(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[3], 14, 0xf4d50d87);
        RG(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[8], 20, 0x455a14ed);
        RG(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[13],  5, 0xa9e3e905);
        RG(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[2],  9, 0xfcefa3f8);
        RG(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[7], 14, 0x676f02d9);
        RG(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[12], 20, 0x8d2a4c8a);

        /* Round 3. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RH(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[5],  4, 0xfffa3942);
        RH(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[8], 11, 0x8771f681);
        RH(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[11], 16, 0x6d9d6122);
        RH(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[14], 23, 0xfde5380c);
        RH(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[1],  4, 0xa4beea44);
        RH(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[4], 11, 0x4bdecfa9);
        RH(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[7], 16, 0xf6bb4b60);
        RH(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[10], 23, 0xbebfbc70);
        RH(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[13],  4, 0x289b7ec6);
        RH(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[0], 11, 0xeaa127fa);
        RH(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[3], 16, 0xd4ef3085);
        RH(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[6], 23, 0x4881d05);
        RH(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[9],  4, 0xd9d4d039);
        RH(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[12], 11, 0xe6db99e5);
        RH(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[15], 16, 0x1fa27cf8);
        RH(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[2], 23, 0xc4ac5665);

        /* Round 4. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        RI(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[0],  6, 0xf4292244);
        RI(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[7], 10, 0x432aff97);
        RI(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[14], 15, 0xab9423a7);
        RI(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[5], 21, 0xfc93a039);
        RI(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[12],  6, 0x655b59c3);
        RI(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[3], 10, 0x8f0ccc92);
        RI(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[10], 15, 0xffeff47d);
        RI(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[1], 21, 0x85845dd1);
        RI(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[8],  6, 0x6fa87e4f);
        RI(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[15], 10, 0xfe2ce6e0);
        RI(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[6], 15, 0xa3014314);
        RI(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[13], 21, 0x4e0811a1);
        RI(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[4],  6, 0xf7537e82);
        RI(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[11], 10, 0xbd3af235);
        RI(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[2], 15, 0x2ad7d2bb);
        RI(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[9], 21, 0xeb86d391);

        mdbuf[0] = mdbuf[0] + mdbuf_prev[0];
        mdbuf[1] = mdbuf[1] + mdbuf_prev[1];
        mdbuf[2] = mdbuf[2] + mdbuf_prev[2];
        mdbuf[3] = mdbuf[3] + mdbuf_prev[3];
    }

    free(buffer);

    /* TODO free memories!! */

#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

    /* TODO idk this feels awkward */
    out_bytes = (uint8_t *) mdbuf;
    for (i = 0; i < 16; i++) {
        printf("%.2x", out_bytes[i]);
    }
    printf("\n");

#endif


#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#error "big endian not implemented (also macros don't work properly)"
    /* TODO big endian */
#endif


    return result;
}
