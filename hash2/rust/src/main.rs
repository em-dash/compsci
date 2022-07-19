use std::fs;
use std::io;
use std::env;

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

fn round_f(a: u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) -> u32 {
    // a = b + (a + AF(b, c, d) + xk + ti).rotate_left(s);
    /* gonna just try dealing with .wrapping_add() because i don't wanna deal
     * with Wrapping<u32> or whatever it is */
    /* wide boi */
    (a.wrapping_add(aux_f(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b)
}

fn round_g(a: u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) -> u32 {
    (a.wrapping_add(aux_g(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b)
}

fn round_h(a: u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) -> u32 {
    (a.wrapping_add(aux_h(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b)
}

fn round_i(a: u32, b: u32, c: u32, d: u32, xk: u32, s: u32, ti: u32) -> u32 {
    (a.wrapping_add(aux_i(b, c, d)).wrapping_add(xk).wrapping_add(ti))
        .rotate_left(s).wrapping_add(b)
}


fn main() -> Result<(), io::Error> {

    // what is that .expect syntax??
    // anyway use a buff reader or something apparently this isnt' proper
    /* will panic if an argument is not valid unicode */
    let args: Vec<String> = env::args().collect();
    /* 
    if args.len() < 2 {
        /* TODO make this print to stderr instead */
        println!("pls pass a filename");
        return Err("no filename");
    }
    */
    
    /* TODO need checks on the filename */
    let mut buffer = fs::read(&args[1]).expect("unable to read file");
    let file_len = buffer.len();
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


    /*
    let mut word_a: u32 = 0x67452301;
    let mut word_b: u32 = 0xefcdab89;
    let mut word_c: u32 = 0x98badcfe;
    let mut word_d: u32 = 0x10325476;
    let mut word_a_prev: u32;
    let mut word_b_prev: u32;
    let mut word_c_prev: u32;
    let mut word_d_prev: u32;
    */

    let mut mdbuf: [u32; 4] = [0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476];
    let mut mdbuf_prev: [u32; 4] = [0; 4];

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

        mdbuf_prev[0] = mdbuf[0];
        mdbuf_prev[1] = mdbuf[1];
        mdbuf_prev[2] = mdbuf[2];
        mdbuf_prev[3] = mdbuf[3];
        

        /* Round 1. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        mdbuf[0] = round_f(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[0],  7, 0xd76aa478);
        mdbuf[3] = round_f(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[1], 12, 0xe8c7b756);
        mdbuf[2] = round_f(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[2], 17, 0x242070db);
        mdbuf[1] = round_f(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[3], 22, 0xc1bdceee);
        mdbuf[0] = round_f(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[4],  7, 0xf57c0faf);
        mdbuf[3] = round_f(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[5], 12, 0x4787c62a);
        mdbuf[2] = round_f(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[6], 17, 0xa8304613);
        mdbuf[1] = round_f(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[7], 22, 0xfd469501);
        mdbuf[0] = round_f(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[8],  7, 0x698098d8);
        mdbuf[3] = round_f(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[9], 12, 0x8b44f7af);
        mdbuf[2] = round_f(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1], op_x[10], 17, 0xffff5bb1);
        mdbuf[1] = round_f(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0], op_x[11], 22, 0x895cd7be);
        mdbuf[0] = round_f(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3], op_x[12],  7, 0x6b901122);
        mdbuf[3] = round_f(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2], op_x[13], 12, 0xfd987193);
        mdbuf[2] = round_f(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1], op_x[14], 17, 0xa679438e);
        mdbuf[1] = round_f(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0], op_x[15], 22, 0x49b40821);

        /* Round 2. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        mdbuf[0] = round_g(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[1],  5, 0xf61e2562);
        mdbuf[3] = round_g(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[6],  9, 0xc040b340);
        mdbuf[2] = round_g(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[11], 14, 0x265e5a51);
        mdbuf[1] = round_g(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[0], 20, 0xe9b6c7aa);
        mdbuf[0] = round_g(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[5],  5, 0xd62f105d);
        mdbuf[3] = round_g(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[10],  9, 0x2441453);
        mdbuf[2] = round_g(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[15], 14, 0xd8a1e681);
        mdbuf[1] = round_g(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[4], 20, 0xe7d3fbc8);
        mdbuf[0] = round_g(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[9],  5, 0x21e1cde6);
        mdbuf[3] = round_g(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[14],  9, 0xc33707d6);
        mdbuf[2] = round_g(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[3], 14, 0xf4d50d87);
        mdbuf[1] = round_g(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[8], 20, 0x455a14ed);
        mdbuf[0] = round_g(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[13],  5, 0xa9e3e905);
        mdbuf[3] = round_g(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[2],  9, 0xfcefa3f8);
        mdbuf[2] = round_g(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[7], 14, 0x676f02d9);
        mdbuf[1] = round_g(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[12], 20, 0x8d2a4c8a);

        /* Round 3. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        mdbuf[0] = round_h(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[5],  4, 0xfffa3942);
        mdbuf[3] = round_h(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[8], 11, 0x8771f681);
        mdbuf[2] = round_h(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[11], 16, 0x6d9d6122);
        mdbuf[1] = round_h(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[14], 23, 0xfde5380c);
        mdbuf[0] = round_h(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[1],  4, 0xa4beea44);
        mdbuf[3] = round_h(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[4], 11, 0x4bdecfa9);
        mdbuf[2] = round_h(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[7], 16, 0xf6bb4b60);
        mdbuf[1] = round_h(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[10], 23, 0xbebfbc70);
        mdbuf[0] = round_h(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[13],  4, 0x289b7ec6);
        mdbuf[3] = round_h(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[0], 11, 0xeaa127fa);
        mdbuf[2] = round_h(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[3], 16, 0xd4ef3085);
        mdbuf[1] = round_h(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[6], 23, 0x4881d05);
        mdbuf[0] = round_h(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[9],  4, 0xd9d4d039);
        mdbuf[3] = round_h(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[12], 11, 0xe6db99e5);
        mdbuf[2] = round_h(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[15], 16, 0x1fa27cf8);
        mdbuf[1] = round_h(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[2], 23, 0xc4ac5665);

        /* Round 4. */
        /* Let [abcd k s i] denote the operation
          a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
        /* Do the following 16 operations. */
        mdbuf[0] = round_i(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[0],  6, 0xf4292244);
        mdbuf[3] = round_i(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[7], 10, 0x432aff97);
        mdbuf[2] = round_i(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[14], 15, 0xab9423a7);
        mdbuf[1] = round_i(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[5], 21, 0xfc93a039);
        mdbuf[0] = round_i(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],  op_x[12],  6, 0x655b59c3);
        mdbuf[3] = round_i(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],   op_x[3], 10, 0x8f0ccc92);
        mdbuf[2] = round_i(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],  op_x[10], 15, 0xffeff47d);
        mdbuf[1] = round_i(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[1], 21, 0x85845dd1);
        mdbuf[0] = round_i(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[8],  6, 0x6fa87e4f);
        mdbuf[3] = round_i(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[15], 10, 0xfe2ce6e0);
        mdbuf[2] = round_i(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[6], 15, 0xa3014314);
        mdbuf[1] = round_i(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],  op_x[13], 21, 0x4e0811a1);
        mdbuf[0] = round_i(mdbuf[0], mdbuf[1], mdbuf[2], mdbuf[3],   op_x[4],  6, 0xf7537e82);
        mdbuf[3] = round_i(mdbuf[3], mdbuf[0], mdbuf[1], mdbuf[2],  op_x[11], 10, 0xbd3af235);
        mdbuf[2] = round_i(mdbuf[2], mdbuf[3], mdbuf[0], mdbuf[1],   op_x[2], 15, 0x2ad7d2bb);
        mdbuf[1] = round_i(mdbuf[1], mdbuf[2], mdbuf[3], mdbuf[0],   op_x[9], 21, 0xeb86d391);

        mdbuf[0] = mdbuf[0].wrapping_add(mdbuf_prev[0]);
        mdbuf[1] = mdbuf[1].wrapping_add(mdbuf_prev[1]);
        mdbuf[2] = mdbuf[2].wrapping_add(mdbuf_prev[2]);
        mdbuf[3] = mdbuf[3].wrapping_add(mdbuf_prev[3]);
    }

    let mut result: Vec<u8> = Vec::new();
    result.extend_from_slice(&(mdbuf[0].to_le_bytes()));
    result.extend_from_slice(&(mdbuf[1].to_le_bytes()));
    result.extend_from_slice(&(mdbuf[2].to_le_bytes()));
    result.extend_from_slice(&(mdbuf[3].to_le_bytes()));

    for i in 0..(result.len()) {
        print!("{:02x}", result[i]);
    }


    println!();


    Ok(())
}
