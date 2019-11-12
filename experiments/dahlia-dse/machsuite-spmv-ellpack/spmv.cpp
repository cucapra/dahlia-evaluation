#include <ap_int.h>
extern "C" {
  void spmv(double nzval[494][10], ap_int<32> cols[494][10], double vec[494], double out[494]) {
    #pragma HLS INTERFACE m_axi port=nzval offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=nzval bundle=control
    #pragma HLS INTERFACE m_axi port=cols offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=cols bundle=control
    #pragma HLS INTERFACE m_axi port=vec offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=vec bundle=control
    #pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=out bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    for(int i = 0; i < 494; i++) {
      for(int j = 0; j < 10; j++) {
        double si = (nzval[i][j] * vec[cols[i][j]]);
        // combiner:
        out[i] += si;
      }
    }
  }
}