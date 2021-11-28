use std::fs;
use std::io;

/* F, G, H, I auxiliary function macros */
fn aux_f(x: u32, y: u32, z: u32) -> u32 {
    x & y | !x & z
}

fn aux_g(x: u32, y: u32, z: u32) -> u32 {
    x & z | y & !z
}

fn aux_h(x: u32, y: u32, z: u32) -> u32 {
    x ^ y ^ z
}

fn aux_i(x: u32, y: u32, z: u32) -> u32 {
    y ^ (x | !z)
}

/* TODO duplicating this because idk what to do as the equivalent to function
 * pointers (to switch out aux_f, aux_g, etc) */
fn round_f(a: &mut u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) {
    // a = b + (a + AF(b, c, d) + xk + ti).rotate_left(s);
    /* gonna just try dealing with .wrapping_add() because i don't wanna deal
     * with Wrapping<u32> or whatever it is */
    /* wide boi */
    *a = (a.wrapping_add(aux_f(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b);
}

fn round_g(a: &mut u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) {
    *a = (a.wrapping_add(aux_g(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b);
}

fn round_h(a: &mut u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) {
    *a = (a.wrapping_add(aux_h(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b);
}

fn round_i(a: &mut u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) {
    *a = (a.wrapping_add(aux_i(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b);
}


fn main() -> Result<(), io::Error> {

    // what is that .expect syntax??
    // anyway use a buff reader or something apparently this isnt' proper
    let mut buffer = fs::read("test").expect("unable to read file");
    let file_len = buffer.len();
    // TODO check this works properly cause i copy/pasted from my C version
    let padded_len = file_len + 8 + (64 - ((file_len + 8) % 64));

    // 128 ends up at buffer[file_len]
    buffer.push(128); // a single 1 bit followed by 0 bits
    /* goes up to buffer[padded_len - 9] */
    buffer.resize(padded_len - 8, 0);

    /* TODO handle size too big for u64.  currently this will panic. */
    /* what is .try_into().unwrap() it sure is very wordy */
    let len_append: u64 = (file_len * 8).try_into().unwrap();
    // let bytes = len_append.to_le_bytes();
    buffer.extend_from_slice(&(len_append.to_le_bytes()));


    let mut word_a: u32 = 0x67452301;
    let mut word_b: u32 = 0xefcdab89;
    let mut word_c: u32 = 0x98badcfe;
    let mut word_d: u32 = 0x10325476;
    let mut word_a_prev: u32;
    let mut word_b_prev: u32;
    let mut word_c_prev: u32;
    let mut word_d_prev: u32;
    /*
    let mut mdbuf: [u32; 4] = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476];
    let mut mdbuf_prev: [u32; 4] = [0; 4];
    */
    let mut op_x: [u32; 16] = [0; 16];

    /* given the weird syntax where 0..8 doesn't include 8, maybe i should just
     * use while loops instead lol */
    for i in 0..(buffer.len() / 64) {
        /* just gonna do this by copying into an array for now, no idea how to
         * do it with pointers to buffer instead (maybe slice and tryinto or
         * something or maybe the compiler does it automatically but that feels
         * awkward) */
        /* can do slices but then either still have to write it to an array (or
         * whatever), or have to convert bytes to u32 every round */

        /* set phasers to slice */
        for j in 0..16 {
            // is there a range() syntax for this
            /* TODO maybe a more idiomatic way to skip forward a number of u32s
             * like that */
            op_x[j] = u32::from_le_bytes(
                (&buffer[(i * 64 + j * 4)..(i * 64 + j * 4 + 4)])
                .try_into().unwrap()
            );
        }
        
        /*
        for j in 0..4 {
            mdbuf_prev[j] = mdbuf[j];
        }
        */

        word_a_prev = word_a;
        word_b_prev = word_b;
        word_c_prev = word_c;
        word_d_prev = word_d;
        

        /* Round 1. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        round_f(&mut word_a, word_b, word_c, word_d,  op_x[0],  7, 0xd76aa478);
        round_f(&mut word_d, word_a, word_b, word_c,  op_x[1], 12, 0xe8c7b756);
        round_f(&mut word_c, word_d, word_a, word_b,  op_x[2], 17, 0x242070db);
        round_f(&mut word_b, word_c, word_d, word_a,  op_x[3], 22, 0xc1bdceee);
        round_f(&mut word_a, word_b, word_c, word_d,  op_x[4],  7, 0xf57c0faf);
        round_f(&mut word_d, word_a, word_b, word_c,  op_x[5], 12, 0x4787c62a);
        round_f(&mut word_c, word_d, word_a, word_b,  op_x[6], 17, 0xa8304613);
        round_f(&mut word_b, word_c, word_d, word_a,  op_x[7], 22, 0xfd469501);
        round_f(&mut word_a, word_b, word_c, word_d,  op_x[8],  7, 0x698098d8);
        round_f(&mut word_d, word_a, word_b, word_c,  op_x[9], 12, 0x8b44f7af);
        round_f(&mut word_c, word_d, word_a, word_b, op_x[10], 17, 0xffff5bb1);
        round_f(&mut word_b, word_c, word_d, word_a, op_x[11], 22, 0x895cd7be);
        round_f(&mut word_a, word_b, word_c, word_d, op_x[12],  7, 0x6b901122);
        round_f(&mut word_d, word_a, word_b, word_c, op_x[13], 12, 0xfd987193);
        round_f(&mut word_c, word_d, word_a, word_b, op_x[14], 17, 0xa679438e);
        round_f(&mut word_b, word_c, word_d, word_a, op_x[15], 22, 0x49b40821);

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        round_g(&mut word_a, word_b, word_c, word_d,   op_x[1],  5, 0xf61e2562);
        round_g(&mut word_d, word_a, word_b, word_c,   op_x[6],  9, 0xc040b340);
        round_g(&mut word_c, word_d, word_a, word_b,  op_x[11], 14, 0x265e5a51);
        round_g(&mut word_b, word_c, word_d, word_a,   op_x[0], 20, 0xe9b6c7aa);
        round_g(&mut word_a, word_b, word_c, word_d,   op_x[5],  5, 0xd62f105d);
        round_g(&mut word_d, word_a, word_b, word_c,  op_x[10],  9, 0x2441453);
        round_g(&mut word_c, word_d, word_a, word_b,  op_x[15], 14, 0xd8a1e681);
        round_g(&mut word_b, word_c, word_d, word_a,   op_x[4], 20, 0xe7d3fbc8);
        round_g(&mut word_a, word_b, word_c, word_d,   op_x[9],  5, 0x21e1cde6);
        round_g(&mut word_d, word_a, word_b, word_c,  op_x[14],  9, 0xc33707d6);
        round_g(&mut word_c, word_d, word_a, word_b,   op_x[3], 14, 0xf4d50d87);
        round_g(&mut word_b, word_c, word_d, word_a,   op_x[8], 20, 0x455a14ed);
        round_g(&mut word_a, word_b, word_c, word_d,  op_x[13],  5, 0xa9e3e905);
        round_g(&mut word_d, word_a, word_b, word_c,   op_x[2],  9, 0xfcefa3f8);
        round_g(&mut word_c, word_d, word_a, word_b,   op_x[7], 14, 0x676f02d9);
        round_g(&mut word_b, word_c, word_d, word_a,  op_x[12], 20, 0x8d2a4c8a);

        /* Round 3. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        round_h(&mut word_a, word_b, word_c, word_d,   op_x[5],  4, 0xfffa3942);
        round_h(&mut word_d, word_a, word_b, word_c,   op_x[8], 11, 0x8771f681);
        round_h(&mut word_c, word_d, word_a, word_b,  op_x[11], 16, 0x6d9d6122);
        round_h(&mut word_b, word_c, word_d, word_a,  op_x[14], 23, 0xfde5380c);
        round_h(&mut word_a, word_b, word_c, word_d,   op_x[1],  4, 0xa4beea44);
        round_h(&mut word_d, word_a, word_b, word_c,   op_x[4], 11, 0x4bdecfa9);
        round_h(&mut word_c, word_d, word_a, word_b,   op_x[7], 16, 0xf6bb4b60);
        round_h(&mut word_b, word_c, word_d, word_a,  op_x[10], 23, 0xbebfbc70);
        round_h(&mut word_a, word_b, word_c, word_d,  op_x[13],  4, 0x289b7ec6);
        round_h(&mut word_d, word_a, word_b, word_c,   op_x[0], 11, 0xeaa127fa);
        round_h(&mut word_c, word_d, word_a, word_b,   op_x[3], 16, 0xd4ef3085);
        round_h(&mut word_b, word_c, word_d, word_a,   op_x[6], 23, 0x4881d05);
        round_h(&mut word_a, word_b, word_c, word_d,   op_x[9],  4, 0xd9d4d039);
        round_h(&mut word_d, word_a, word_b, word_c,  op_x[12], 11, 0xe6db99e5);
        round_h(&mut word_c, word_d, word_a, word_b,  op_x[15], 16, 0x1fa27cf8);
        round_h(&mut word_b, word_c, word_d, word_a,   op_x[2], 23, 0xc4ac5665);

        /* Round 4. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        round_i(&mut word_a, word_b, word_c, word_d,   op_x[0],  6, 0xf4292244);
        round_i(&mut word_d, word_a, word_b, word_c,   op_x[7], 10, 0x432aff97);
        round_i(&mut word_c, word_d, word_a, word_b,  op_x[14], 15, 0xab9423a7);
        round_i(&mut word_b, word_c, word_d, word_a,   op_x[5], 21, 0xfc93a039);
        round_i(&mut word_a, word_b, word_c, word_d,  op_x[12],  6, 0x655b59c3);
        round_i(&mut word_d, word_a, word_b, word_c,   op_x[3], 10, 0x8f0ccc92);
        round_i(&mut word_c, word_d, word_a, word_b,  op_x[10], 15, 0xffeff47d);
        round_i(&mut word_b, word_c, word_d, word_a,   op_x[1], 21, 0x85845dd1);
        round_i(&mut word_a, word_b, word_c, word_d,   op_x[8],  6, 0x6fa87e4f);
        round_i(&mut word_d, word_a, word_b, word_c,  op_x[15], 10, 0xfe2ce6e0);
        round_i(&mut word_c, word_d, word_a, word_b,   op_x[6], 15, 0xa3014314);
        round_i(&mut word_b, word_c, word_d, word_a,  op_x[13], 21, 0x4e0811a1);
        round_i(&mut word_a, word_b, word_c, word_d,   op_x[4],  6, 0xf7537e82);
        round_i(&mut word_d, word_a, word_b, word_c,  op_x[11], 10, 0xbd3af235);
        round_i(&mut word_c, word_d, word_a, word_b,   op_x[2], 15, 0x2ad7d2bb);
        round_i(&mut word_b, word_c, word_d, word_a,   op_x[9], 21, 0xeb86d391);

        word_a = word_a.wrapping_add(word_a_prev);
        word_b = word_b.wrapping_add(word_b_prev);
        word_c = word_c.wrapping_add(word_c_prev);
        word_d = word_d.wrapping_add(word_d_prev);
    }

    let mut result: Vec<u8> = Vec::new();
    result.extend_from_slice(&(word_a.to_le_bytes()));
    result.extend_from_slice(&(word_b.to_le_bytes()));
    result.extend_from_slice(&(word_c.to_le_bytes()));
    result.extend_from_slice(&(word_d.to_le_bytes()));

    for i in 0..(result.len()) {
        print!("{:02x}", result[i]);
    }


    println!();


    Ok(())
}
