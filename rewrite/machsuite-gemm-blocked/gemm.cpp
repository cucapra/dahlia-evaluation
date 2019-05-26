template < int N >
using ap_int = int;

void gemm(double m1[4096], double m2[4096], double prod[4096]) {

  double temp_x = 0.0;
  double mul = 0.0;
  ap_int<32> i_row = 0;
  ap_int<32> k_row = 0;
  ap_int<32> jj = 0;
  while((jj < 64)) {
    ap_int<32> kk = 0;
    while((kk < 64)) {
      for(int i = 0; i < 64; i++) {

        for(int k = 0; k < 8; k++) {

          i_row = (i * 64);
          k_row = ((k + kk) * 64);
          temp_x = m1[(i_row + (k + kk))];
          for(int j = 0; j < 8; j++) {

            mul = (temp_x * m2[(k_row + (j + jj))]);
            double temp = prod[(i_row + (j + jj))];
            //---
            prod[(i_row + (j + jj))] = (temp + mul);

          }

        }

      }
      kk = (kk + 8);
    }
    jj = (jj + 8);
  }
}
