#include <ap_int.h>
ap_uint<8> rj_xtime(ap_uint<8> x) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  if (((x & 128) != 0)) {
    return ((x << 1) ^ 27);
  } else {
    return (x << 1);
  }
}
void aes_subBytes(ap_uint<8> buf[16], ap_uint<8> sbox[256]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  for(int i = 0; i < 16; i++) {
    ap_uint<8> read = buf[i];
    //---
    buf[i] = sbox[read];
  }
}
void aes_addRoundKey(ap_uint<8> buf[16], ap_uint<8> key[32], ap_uint<1> offset) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  for(int i = 0; i < 16; i++) {
    ap_uint<8> read = buf[i];
    //---
    if ((offset == 0)) {
      buf[i] = (read ^ key[i]);
    } else {
      buf[i] = (read ^ key[(i + 16)]);
    }
  }
}
void aes_addRoundKey_cpy(ap_uint<8> buf[16], ap_uint<8> key[32], ap_uint<8> cpk[32]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  for(int i = 0; i < 16; i++) {
    ap_uint<8> read = buf[i];
    ap_uint<8> _key = key[i];
    cpk[i] = _key;
    //---
    buf[i] = (read ^ _key);
    cpk[(16 + i)] = key[(16 + i)];
  }
}
void aes_shiftRows(ap_uint<8> buf[16]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  ap_uint<8> temp = buf[1];
  //---
  ap_uint<8> read = buf[5];
  //---
  buf[1] = read;
  //---
  read = buf[9];
  //---
  buf[5] = read;
  //---
  read = buf[13];
  //---
  buf[9] = read;
  //---
  buf[13] = temp;
  //---
  temp = buf[10];
  //---
  read = buf[2];
  //---
  buf[10] = read;
  //---
  buf[2] = temp;
  //---
  temp = buf[3];
  //---
  read = buf[15];
  //---
  buf[3] = read;
  //---
  read = buf[11];
  //---
  buf[15] = read;
  //---
  read = buf[7];
  //---
  buf[11] = read;
  //---
  buf[7] = temp;
  //---
  temp = buf[14];
  //---
  read = buf[6];
  //---
  buf[14] = read;
  //---
  buf[6] = temp;
}
void aes_mixColumns(ap_uint<8> buf[16]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  for(int i = 0; i < 4; i++) {
    ap_uint<8> a = buf[(4 * i)];
    //---
    ap_uint<8> b = buf[((4 * i) + 1)];
    //---
    ap_uint<8> c = buf[((4 * i) + 2)];
    //---
    ap_uint<8> d = buf[((4 * i) + 3)];
    //---
    ap_uint<8> e = (((a ^ b) ^ c) ^ d);
    //---
    ap_uint<8> temp = rj_xtime((a ^ b));
    buf[(4 * i)] = ((a ^ e) ^ temp);
    //---
    temp = rj_xtime((b ^ c));
    buf[((4 * i) + 1)] = ((b ^ e) ^ temp);
    //---
    temp = rj_xtime((c ^ d));
    buf[((4 * i) + 2)] = ((c ^ e) ^ temp);
    //---
    temp = rj_xtime((d ^ a));
    buf[((4 * i) + 3)] = ((d ^ e) ^ temp);
  }
}
void aes_expandEncKey(ap_uint<8> k[32], ap_uint<8> rc, ap_uint<8> sbox[256]) {
  #pragma HLS INLINE
  #pragma HLS INTERFACE s_axilite port=return bundle=control
  ap_uint<8> _k = k[0];
  //---
  ap_uint<8> _rj = (sbox[k[29]] ^ rc);
  //---
  k[0] = (_k ^ _rj);
  //---
  _k = k[1];
  //---
  _rj = sbox[k[30]];
  //---
  k[1] = (_k ^ _rj);
  //---
  _k = k[2];
  //---
  _rj = sbox[k[31]];
  //---
  k[2] = (_k ^ _rj);
  //---
  _k = k[3];
  //---
  _rj = sbox[k[28]];
  //---
  k[3] = (_k ^ _rj);
  //---
  for(int i = 4; i < 16; i++) {
    _k = k[i];
    //---
    _rj = k[(i - 4)];
    //---
    k[i] = (_k ^ _rj);
  }
  //---
  _k = k[16];
  //---
  _rj = sbox[k[12]];
  //---
  k[16] = (_k ^ _rj);
  //---
  _k = k[17];
  //---
  _rj = sbox[k[13]];
  //---
  k[17] = (_k ^ _rj);
  //---
  _k = k[18];
  //---
  _rj = sbox[k[14]];
  //---
  k[18] = (_k ^ _rj);
  //---
  _k = k[19];
  //---
  _rj = sbox[k[15]];
  //---
  k[19] = (_k ^ _rj);
  //---
  for(int i = 20; i < 32; i++) {
    _k = k[i];
    //---
    _rj = k[(i - 4)];
    //---
    k[i] = (_k ^ _rj);
  }
}
extern "C" {
  void aes(ap_uint<8> key[32], ap_uint<8> enckey[32], ap_uint<8> deckey[32], ap_uint<8> k[32], ap_uint<8> buf[16]) {
    #pragma HLS INTERFACE m_axi port=key offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=key bundle=control
    #pragma HLS INTERFACE m_axi port=enckey offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=enckey bundle=control
    #pragma HLS INTERFACE m_axi port=deckey offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=deckey bundle=control
    #pragma HLS INTERFACE m_axi port=k offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=k bundle=control
    #pragma HLS INTERFACE m_axi port=buf offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=buf bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_uint<8> sbox[256] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x1, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x4, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x5, 0x9a, 0x7, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x9, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x0, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x2, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0xc, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0xb, 0xdb, 0xe0, 0x32, 0x3a, 0xa, 0x49, 0x6, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x8, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x3, 0xf6, 0xe, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0xd, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0xf, 0xb0, 0x54, 0xbb, 0x16};
    ap_uint<8> rcon = 1;
    for(int i = 0; i < 32; i++) {
      enckey[i] = k[i];
      deckey[i] = k[i];
    }
    //---
    for(int i = 0; i < 7; i++) {
      aes_expandEncKey(deckey, rcon, sbox);
      rcon = ((rcon << 1) ^ (((rcon >> 7) & 1) * 27));
    }
    aes_addRoundKey_cpy(buf, enckey, key);
    //---
    rcon = 1;
    for(int i = 1; i < 14; i++) {
      aes_subBytes(buf, sbox);
      //---
      aes_shiftRows(buf);
      //---
      aes_mixColumns(buf);
      //---
      if (((i & 1) != 0)) {
        aes_addRoundKey(buf, key, 1);
      } else {
        aes_expandEncKey(key, rcon, sbox);
        rcon = ((rcon << 1) ^ (((rcon >> 7) & 1) * 27));
        //---
        aes_addRoundKey(buf, key, 0);
      }
    }
    //---
    aes_subBytes(buf, sbox);
    //---
    aes_shiftRows(buf);
    //---
    aes_expandEncKey(key, rcon, sbox);
    //---
    aes_addRoundKey(buf, key, 0);
  }
}