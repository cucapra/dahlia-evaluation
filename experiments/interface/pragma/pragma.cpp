#include <ap_int.h>

extern "C"
{
  void partition(ap_int<32> A[1024], ap_int<32> out[1]) {
    #pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = A offset = slave bundle = gmem
    #pragma hls interface s_axilite port = out bundle = control
    #pragma HLS INTERFACE s_axilite port = A bundle = control
    #pragma HLS INTERFACE s_axilite port = return bundle = control

    #pragma HLS ARRAY_PARTITION variable = A factor = 4 dim = 1

    for (int i = 0; i < 1024; i++) {
      out[0] += A[i];
    }
  }
}
