#include <ap_int.h>
extern "C" {
  void stencil(ap_int<32> orig[128][64], ap_int<32> sol[128][64], ap_int<32> filter[3][3]) {
    #pragma HLS INTERFACE m_axi port=orig offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=orig bundle=control
    #pragma HLS INTERFACE m_axi port=sol offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=sol bundle=control
    #pragma HLS INTERFACE m_axi port=filter offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=filter bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    for(int r = 0; r < 126; r++) {
      for(int c = 0; c < 62; c++) {
        ap_int<32> temp = 0;
        for(int k1 = 0; k1 < 3; k1++) {
          ap_int<32> temp_2 = 0;
          for(int k2 = 0; k2 < 3; k2++) {
            ap_int<32> mul = (filter[k1][k2] * orig[(r + k1)][(c + k2)]);
            // combiner:
            temp_2 += mul;
          }
          // combiner:
          temp += temp_2;
        }
        sol[r][c] = temp;
      }
    }
  }
}