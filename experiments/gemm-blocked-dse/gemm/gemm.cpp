// git.status = clean, build.date = Thu Nov 07 23:55:57 EST 2019, git.hash = 8f3df50
#include <ap_int.h>
extern "C" {
  void gemm(ap_int<32> m1_int[128][128], ap_int<32> m2_int[128][128], ap_int<32> prod_int[128][128]) {
    #pragma HLS INTERFACE m_axi port=m1_int offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m1_int bundle=control
    #pragma HLS INTERFACE m_axi port=m2_int offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=m2_int bundle=control
    #pragma HLS INTERFACE m_axi port=prod_int offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=prod_int bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_int<32> m1[128][128];
    #pragma HLS resource variable=m1 core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=::BANK11:: dim=1
    #pragma HLS ARRAY_PARTITION variable=m1 cyclic factor=::BANK12:: dim=2
    ap_int<32> m2[128][128];
    #pragma HLS resource variable=m2 core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=::BANK11:: dim=1
    #pragma HLS ARRAY_PARTITION variable=m2 cyclic factor=::BANK12:: dim=2
    ap_int<32> prod[128][128];
    #pragma HLS resource variable=prod core=RAM_1P_BRAM
    #pragma HLS ARRAY_PARTITION variable=prod cyclic factor=::BANK21:: dim=1
    #pragma HLS ARRAY_PARTITION variable=prod cyclic factor=::BANK22:: dim=2



    for(int i = 0; i < 128; i++) {
      for(int j = 0; j < 128; j++) {
        m1[i][j] = m1_int[i][j];
        m2[i][j] = m2_int[i][j];
        prod[i][j] = 0;
      }
    }
    //---
    for(int jj = 0; jj < 16; jj++) {
      for(int kk = 0; kk < 16; kk++) {
        for(int i = 0; i < 128; i++) {
          #pragma HLS UNROLL factor=::UNROLL1:: skip_exit_check
          for(int j = 0; j < 8; j++) {
            #pragma HLS UNROLL factor=::UNROLL2:: skip_exit_check
            for(int k = 0; k < 8; k++) {
              #pragma HLS UNROLL factor=::UNROLL3:: skip_exit_check
              ap_int<32> mul = (m1[i][((8 * kk) + k)] * m2[((8 * kk) + k)][((8 * jj) + j)]);
              // combiner:
              prod[i][((8 * jj) + j)] += mul;
            }
          }
        }
      }
    }
    //---
    for(int i = 0; i < 128; i++) {
      for(int j = 0; j < 128; j++) {
        prod_int[i][j] = prod[i][j];
      }
    }
  }
}
