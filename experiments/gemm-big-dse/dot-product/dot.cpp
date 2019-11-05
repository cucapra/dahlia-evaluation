// git.status = clean, build.date = Mon Nov 04 17:05:30 EST 2019, git.hash = 9e2d8b7
#include <ap_int.h>
extern "C" {
  void dot(ap_int<32> m1_int[4096], ap_int<32> m2_int[4096], ap_int<64> prod[1]) {
    #pragma HLS INTERFACE m_axi port=m1_int offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m1_int bundle=control
    #pragma HLS INTERFACE m_axi port=m2_int offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m2_int bundle=control
    #pragma HLS INTERFACE m_axi port=prod offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=prod bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_int<32> m1[4096];
    #pragma HLS resource variable=m1 core=::RESOURCE1::
    #pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=::BANK1:: dim=1
    ap_int<32> m2[4096];
    #pragma HLS resource variable=m2 core=::RESOURCE2::
    #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=::BANK2:: dim=1


    for(int i = 0; i < 4096; i++) {
      m1[i] = m1_int[i];
      m2[i] = m2_int[i];
    }
    //---
    for(int i = 0; i < 4096; i++) {
      #pragma HLS UNROLL factor=::UNROLL:: skip_exit_check

      ap_int<32> x = (m1[i] * m2[i]);
      // combiner:
      prod[0] += x;
    }
  }
}
