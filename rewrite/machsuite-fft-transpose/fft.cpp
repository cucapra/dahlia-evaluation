// git.status = clean, build.date = Sat Nov 09 13:00:37 EST 2019, git.hash = 75aa1ab
#include <ap_int.h>
#include "math.h"
void fft8(double a_x[8], double a_y[8]) {
  // #pragma HLS INLINE
  // #pragma HLS resource variable=a_x core=RAM_1P_BRAM
  // #pragma HLS resource variable=a_y core=RAM_1P_BRAM
  double exp_p1 = 1.0;
  double exp_n1 = -1.0;
  double exp_0 = 0.0;
  double exp_1_44_x = 0.0;
  double exp_1_44_y = -1.0;
  double c0_x = a_x[0];
  double c0_y = a_y[0];
  //---
  double c4_x = a_x[4];
  double c4_y = a_y[4];
  //---
  a_x[0] = (c0_x + c4_x);
  a_y[0] = (c0_y + c4_y);
  //---
  a_x[4] = (c0_x - c4_x);
  a_y[4] = (c0_y - c4_y);
  //---
  double c1_x = a_x[1];
  double c1_y = a_y[1];
  //---
  double c5_x = a_x[5];
  double c5_y = a_y[5];
  //---
  a_x[1] = (c1_x + c5_x);
  a_y[1] = (c1_y + c5_y);
  //---
  a_x[5] = (c1_x - c5_x);
  a_y[5] = (c1_y - c5_y);
  //---
  double c2_x = a_x[2];
  double c2_y = a_y[2];
  //---
  double c6_x = a_x[6];
  double c6_y = a_y[6];
  //---
  a_x[2] = (c2_x + c6_x);
  a_y[2] = (c2_y + c6_y);
  //---
  a_x[6] = (c2_x - c6_x);
  a_y[6] = (c2_y - c6_y);
  //---
  double c3_x = a_x[3];
  double c3_y = a_y[3];
  //---
  double c7_x = a_x[7];
  double c7_y = a_y[7];
  //---
  a_x[3] = (c3_x + c7_x);
  a_y[3] = (c3_y + c7_y);
  //---
  a_x[7] = (c3_x - c7_x);
  a_y[7] = (c3_y - c7_y);
  //---
  c0_x = a_x[0];
  c0_y = a_y[0];
  //---
  c2_x = a_x[2];
  c2_y = a_y[2];
  //---
  a_x[0] = (c0_x + c2_x);
  a_y[0] = (c0_y + c2_y);
  //---
  a_x[2] = (c0_x - c2_x);
  a_y[2] = (c0_y - c2_y);
  //---
  c1_x = a_x[1];
  c1_y = a_y[1];
  //---
  c3_x = a_x[3];
  c3_y = a_y[3];
  //---
  a_x[1] = (c1_x + c3_x);
  a_y[1] = (c1_y + c3_y);
  //---
  double b3_x = (c1_x - c3_x);
  double b3_y = (c1_y - c3_y);
  c3_x = ((b3_x * exp_1_44_x) - (b3_y * exp_1_44_y));
  c3_y = ((b3_x * exp_1_44_y) - (b3_y * exp_1_44_x));
  c0_x = a_x[0];
  c0_y = a_y[0];
  //---
  c1_x = a_x[1];
  c1_y = a_y[1];
  //---
  a_x[0] = (c0_x + c1_x);
  a_y[0] = (c0_y + c1_y);
  //---
  a_x[1] = (c0_x - c1_x);
  a_y[1] = (c0_y - c1_y);
  //---
  c2_x = a_x[2];
  c2_y = a_y[2];
  //---
  a_x[2] = (c2_x + c3_x);
  a_y[2] = (c2_y + c3_y);
  //---
  a_x[3] = (c2_x - c3_x);
  a_y[3] = (c2_y - c3_y);
  //---
  c4_x = a_x[4];
  c4_y = a_y[4];
  //---
  c6_x = a_x[6];
  c6_y = a_y[6];
  double b6_x = ((c6_x * exp_0) - (c6_y * exp_n1));
  double b6_y = ((c6_x * exp_n1) + (c6_y * exp_0));
  //---
  a_x[4] = (c4_x + b6_x);
  a_y[4] = (c4_y + b6_y);
  //---
  a_x[6] = (c4_x - b6_x);
  a_y[6] = (c4_y - b6_y);
  //---
  c5_x = a_x[5];
  c5_y = a_y[5];
  double b5_x = (((c5_x * exp_p1) - (c5_y * exp_n1)) * 0.70710678118654752440);
  double b5_y = (((c5_x * exp_n1) + (c5_y * exp_p1)) * 0.70710678118654752440);
  //---
  c7_x = a_x[7];
  c7_y = a_y[7];
  double b7_x = (((c7_x * exp_n1) - (c7_y * exp_n1)) * 0.70710678118654752440);
  double b7_y = (((c7_x * exp_n1) + (c7_y * exp_n1)) * 0.70710678118654752440);
  //---
  a_x[5] = (b5_x + b7_x);
  a_y[5] = (b5_y + b7_y);
  //---
  b3_x = (b5_x - b7_x);
  b3_y = (b5_y - b7_y);
  c7_x = ((b3_x * exp_1_44_x) - (b3_y * exp_1_44_y));
  c7_y = ((b3_x * exp_1_44_y) - (b3_y * exp_1_44_x));
  c4_x = a_x[4];
  c4_y = a_y[4];
  //---
  c5_x = a_x[5];
  c5_y = a_y[5];
  //---
  a_x[4] = (c4_x + c5_x);
  a_y[4] = (c4_y + c5_y);
  //---
  a_x[5] = (c4_x - c5_x);
  a_y[5] = (c4_y - c5_y);
  //---
  c6_x = a_x[6];
  c6_y = a_y[6];
  //---
  a_x[6] = (c6_x + c7_x);
  a_y[6] = (c6_y + c7_y);
  //---
  a_x[7] = (c6_x - c7_x);
  a_y[7] = (c6_y - c7_y);
}
void twiddles8(double a_x[8], double a_y[8], ap_int<32> i, ap_int<32> n) {
  // #pragma HLS INLINE
  // #pragma HLS resource variable=a_x core=RAM_1P_BRAM
  // #pragma HLS resource variable=a_y core=RAM_1P_BRAM
  ap_int<32> reversed8[8] = {0, 4, 2, 6, 1, 5, 3, 7};
  // #pragma HLS resource variable=reversed8 core=RAM_1P_BRAM
  double PI = 3.1415926535;
  for(int j = 1; j < 8; j++) {
    double phi = (((((double)(0 - 2) * PI) * (double)reversed8[j]) / (double)n) * (double)i);
    double phi_x = cos(phi);
    double phi_y = sin(phi);
    double tmp_x = a_x[j];
    double tmp_y = a_y[j];
    //---
    a_x[j] = ((tmp_x * phi_x) - (tmp_y * phi_y));
    a_y[j] = ((tmp_x * phi_y) + (tmp_y * phi_x));
  }
}
void loadx8(double a_x[8], double x[576], ap_int<32> offset, ap_int<32> sx) {
  // #pragma HLS INLINE
  // #pragma HLS resource variable=a_x core=RAM_1P_BRAM
  // #pragma HLS resource variable=x core=RAM_1P_BRAM
  for(int i = 0; i < 8; i++) {
    a_x[i] = x[((i * sx) + offset)];
  }
}
void loady8(double a_y[8], double x[576], ap_int<32> offset, ap_int<32> sx) {
  // #pragma HLS INLINE
  // #pragma HLS resource variable=a_y core=RAM_1P_BRAM
  // #pragma HLS resource variable=x core=RAM_1P_BRAM
  for(int i = 0; i < 8; i++) {
    a_y[i] = x[((i * sx) + offset)];
  }
}
#pragma SDS data zero_copy(work_x[0:512], work_y[0:512])
extern "C" {
  void fft(double work_x[512], double work_y[512]) {
    #pragma HLS INTERFACE m_axi port=work_x offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=work_x bundle=control
    #pragma HLS INTERFACE m_axi port=work_y offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=work_y bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    double DATA_x[512];
    // #pragma HLS resource variable=DATA_x core=RAM_1P_BRAM
    double DATA_y[512];
    // #pragma HLS resource variable=DATA_y core=RAM_1P_BRAM
    double data_x[8];
    // #pragma HLS resource variable=data_x core=RAM_1P_BRAM
    double data_y[8];
    // #pragma HLS resource variable=data_y core=RAM_1P_BRAM
    double smem[576];
    // #pragma HLS resource variable=smem core=RAM_1P_BRAM
    ap_int<32> reversed8[8] = {0, 4, 2, 6, 1, 5, 3, 7};
    // #pragma HLS resource variable=reversed8 core=RAM_1P_BRAM
    ap_int<32> access_1[8] = {0, 4, 1, 5, 2, 6, 3, 7};
    // #pragma HLS resource variable=access_1 core=RAM_1P_BRAM
    ap_int<32> access_2[8] = {0, 1, 4, 5, 2, 3, 6, 7};
    // #pragma HLS resource variable=access_2 core=RAM_1P_BRAM
    for(int tid = 0; tid < 64; tid++) {
      for(int i = 0; i < 8; i++) {
        data_x[i] = work_x[((i * 64) + tid)];
        data_y[i] = work_y[((i * 64) + tid)];
      }
      //---
      fft8(data_x, data_y);
      //---
      twiddles8(data_x, data_y, tid, 512);
      //---
      for(int i = 0; i < 8; i++) {
        DATA_x[((tid * 8) + i)] = data_x[i];
        DATA_y[((tid * 8) + i)] = data_y[i];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<6> offset = (((tid >> 3) * 8) + (tid & 7));
      for(int i = 0; i < 8; i++) {
        smem[((access_1[i] * 66) + offset)] = DATA_x[((tid * 8) + access_2[i])];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<7> offset = (((tid & 7) * 66) + (tid >> 3));
      for(int i = 0; i < 8; i++) {
        DATA_x[((tid * 8) + access_1[i])] = smem[((access_1[i] * 8) + offset)];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<6> offset = (((tid >> 3) * 8) + (tid & 7));
      for(int i = 0; i < 8; i++) {
        smem[((access_1[i] * 66) + offset)] = DATA_y[((tid * 8) + access_2[i])];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      for(int i = 0; i < 8; i++) {
        data_y[i] = DATA_y[((tid * 8) + i)];
      }
      //---
      loady8(data_y, smem, (((tid & 7) * 66) + (tid >> 3)), 8);
      //---
      for(int i = 0; i < 8; i++) {
        DATA_y[((tid * 8) + i)] = data_y[i];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      for(int i = 0; i < 8; i++) {
        data_x[i] = DATA_x[((tid * 8) + i)];
        data_y[i] = DATA_y[((tid * 8) + i)];
      }
      //---
      fft8(data_x, data_y);
      //---
      twiddles8(data_x, data_y, (tid >> 3), 64);
      //---
      for(int i = 0; i < 8; i++) {
        DATA_x[((tid * 8) + i)] = data_x[i];
        DATA_y[((tid * 8) + i)] = data_y[i];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<6> offset = (((tid >> 3) * 8) + (tid & 7));
      for(int i = 0; i < 8; i++) {
        smem[((access_1[i] * 72) + offset)] = DATA_x[((tid * 8) + access_2[i])];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<7> offset = (((tid >> 3) * 72) + (tid & 7));
      for(int i = 0; i < 8; i++) {
        DATA_x[((tid * 8) + access_1[i])] = smem[((access_1[i] * 8) + offset)];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      ap_int<6> offset = (((tid >> 3) * 8) + (tid & 7));
      for(int i = 0; i < 8; i++) {
        smem[((access_1[i] * 72) + offset)] = DATA_y[((tid * 8) + access_2[i])];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      for(int i = 0; i < 8; i++) {
        data_y[i] = DATA_y[((tid * 8) + i)];
      }
      //---
      loady8(data_y, smem, (((tid >> 3) * 72) + (tid & 7)), 8);
      //---
      for(int i = 0; i < 8; i++) {
        DATA_y[((tid * 8) + i)] = data_y[i];
      }
    }
    //---
    for(int tid = 0; tid < 64; tid++) {
      for(int i = 0; i < 8; i++) {
        data_x[i] = DATA_x[((tid * 8) + i)];
        data_y[i] = DATA_y[((tid * 8) + i)];
      }
      //---
      fft8(data_x, data_y);
      //---
      for(int i = 0; i < 8; i++) {
        work_x[((i * 64) + tid)] = data_x[reversed8[i]];
        work_y[((i * 64) + tid)] = data_y[reversed8[i]];
      }
    }
  }
}
