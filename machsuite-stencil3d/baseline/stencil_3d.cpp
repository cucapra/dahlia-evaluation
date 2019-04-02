/*
Implementation based on algorithm described in:
"Stencil computation optimization and auto-tuning on state-of-the-art multicore architectures"
K. Datta, M. Murphy, V. Volkov, S. Williams, J. Carter, L. Oliker, D. Patterson, J. Shalf, K. Yelick
SC 2008
*/

#include "stencil_3d.h"

void stencil_3d(TYPE C[2], TYPE orig[SIZE], TYPE sol[SIZE]) {
  int i, j, k;
  TYPE sum0, sum1, mul0, mul1;
  TYPE origbuf[SIZE];
#pragma HLS array_partition variable=origbuf cyclic factor=2 
     
  for(i = 0; i < SIZE; i++) {
#pragma HLS PIPELINE
    origbuf[i] = orig[i];
  }
  // Handle boundary conditions by filling with original values
  height_bound_col: for(j=0; j<col_size; j++) {
    height_bound_row: for(k=0; k<row_size; k++) {
      sol[INDX(row_size, col_size, k, j, 0)] = origbuf[INDX(row_size, col_size, k, j, 0)];
      sol[INDX(row_size, col_size, k, j, height_size-1)] = origbuf[INDX(row_size, col_size, k, j, height_size-1)];
    }
  }
  col_bound_height : for(i=1; i<height_size-1; i++) {
    col_bound_row : for(k=0; k<row_size; k++) {
      sol[INDX(row_size, col_size, k, 0, i)] = origbuf[INDX(row_size, col_size, k, 0, i)];
      sol[INDX(row_size, col_size, k, col_size-1, i)] = origbuf[INDX(row_size, col_size, k, col_size-1, i)];
    }
  }
  row_bound_height: for(i=1; i<height_size-1; i++) {
    row_bound_col: for(j=1; j<col_size-1; j++) {
      sol[INDX(row_size, col_size, 0, j, i)] = origbuf[INDX(row_size, col_size, 0, j, i)];
      sol[INDX(row_size, col_size, row_size-1, j, i)] = origbuf[INDX(row_size, col_size, row_size-1, j, i)];
    }
  }


  // Stencil computation
  loop_height: for(i = 1; i < height_size - 1; i++) {
    #pragma HLS pipeline
    loop_col: for(j = 1; j < col_size - 1; j++) {
      #pragma HLS pipeline
      loop_row: for(k = 1; k < row_size - 1; k++) {
        #pragma HLS pipeline
        sum0 = origbuf[INDX(row_size, col_size, k, j, i)];
        sum1 = origbuf[INDX(row_size, col_size, k, j, i + 1)] +
               origbuf[INDX(row_size, col_size, k, j, i - 1)] +
               origbuf[INDX(row_size, col_size, k, j + 1, i)] +
               origbuf[INDX(row_size, col_size, k, j - 1, i)] +
               origbuf[INDX(row_size, col_size, k + 1, j, i)] +
               origbuf[INDX(row_size, col_size, k - 1, j, i)];
        mul0 = sum0 * C[0];
        mul1 = sum1 * C[1];
        sol[INDX(row_size, col_size, k, j, i)] = mul0 + mul1;
      }
    }
  }
}
