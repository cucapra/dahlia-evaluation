

void spmv(float val[1666], ap_int<32> cols[1666], ap_int<32> row_delimiters[495], float vec[494], float out[494]) {
  
  float sum = 0.0;
  float si = 0.0;
  ap_int<32> tmp_begin = 0;
  ap_int<32> tmp_end = 0;
  ap_int<32> j = 0;
  for(int i = 0; i < 494; i++) {
    sum = 0.0;
    si = 0.0;
    tmp_begin = row_delimiters[i];
    //---
    tmp_end = row_delimiters[(i + 1)];
    j = tmp_begin;
    while((j < tmp_end)) {
      si = (val[j] * vec[cols[j]]);
      sum = (sum + si);
      j = (j + 1);
    }
    out[i] = sum;
  }
}