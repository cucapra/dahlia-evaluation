// Avoid using `ap_int` in "software" compilation.
#include "pragma.h"

ap_int<32> A[32][32];
ap_int<32> B[32][32];

ap_int<32> res;

extern "C"
{
  void partition(ap_int<32> iterations, ap_int<32> out[1])
  {
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = iterations bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS ARRAY_PARTITION variable = A factor = 2 dim = 1
#pragma HLS ARRAY_PARTITION variable = A factor = 2 dim = 2

#pragma HLS ARRAY_PARTITION variable = B factor = 2 dim = 1
#pragma HLS ARRAY_PARTITION variable = B factor = 2 dim = 2

    while ((iterations > 0))
    {
#pragma HLS loop_tripcount avg = 1000
      res = 0;
      for (int i = 0; i < 32; i++)
      {
#pragma HLS loop_tripcount avg = 32
        for (int j = 0; j < 32; j++)
        {
#pragma HLS loop_tripcount avg = 32
          A[i][j] = 0;
          B[i][j] = (i + j);
        }
      }
      //---
      for (int i = 0; i < 32; i++)
      {
#pragma HLS loop_tripcount avg = 32
        for (int j = 0; j < 32; j++)
        {
#pragma HLS loop_tripcount avg = 32
          A[i][j] = (B[i][j] + 1);
        }
      }
      //---
      for (int i = 0; i < 32; i++)
      {
#pragma HLS loop_tripcount avg = 32
        for (int j = 0; j < 32; j++)
        {
#pragma HLS loop_tripcount avg = 32
          ap_int<32> x = B[i][j];
          // combiner:
          res += x;
        }
      }
      iterations = (iterations - 1);
    }

    out[0] = res;

  }
}
