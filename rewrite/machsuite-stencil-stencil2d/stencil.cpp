template < int N >
using ap_int = int;

void stencil(ap_int<32> orig[8192], ap_int<32> sol[8192], ap_int<32> filter[9]) {

  ap_int<32> temp = 0;
  ap_int<32> mul = 0;
  for(int r = 0; r < 126; r++) {
    for(int c = 0; c < 62; c++) {
      temp = 0;
      for(int k1 = 0; k1 < 3; k1++) {
        for(int k2 = 0; k2 < 3; k2++) {
          mul = (filter[((k1 * 3) + k2)] * orig[(((r + k1) * 64) + (c + k2))]);
          temp = (temp + mul);
        }
      }
      sol[((r * 64) + c)] = temp;
    }
  }
}
