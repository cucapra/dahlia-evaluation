
void merge(ap_int<32> a[2048], ap_int<16> start, ap_int<16> m, ap_int<16> stop) {
  
  ap_int<32> temp[2048];
  ap_int<16> i = start;
  ap_int<16> j = (m + 1);
  //---
  while((i <= m)) {
    temp[i] = a[i];
    i = (i + 1);
  }
  //---
  while((j <= stop)) {
    temp[(m + (1 + (stop - j)))] = a[j];
    j = (j + 1);
  }
  //---
  ap_int<16> k = start;
  i = start;
  j = stop;
  //---
  while((k <= stop)) {
    ap_int<32> temp_j = 0;
    ap_int<32> temp_i = 0;
    temp_j = temp[j];
    //---
    temp_i = temp[i];
    //---
    if((temp_j < temp_i)){
      a[k] = temp_j;
      j = (j - 1);
    } else{
      a[k] = temp_i;
      i = (i + 1);
    }
    k = (k + 1);
  }
}
void kernel(ap_int<32> a[2048]) {
  
  ap_int<16> start = 0;
  ap_int<16> stop = 2048;
  ap_int<16> m = 1;
  ap_int<16> from = 0;
  ap_int<16> mid = 0;
  ap_int<16> to = 0;
  //---
  while((m < (stop - start))) {
    ap_int<16> i = start;
    while((i < stop)) {
      from = i;
      mid = (i + (m - 1));
      to = (i + (m + (m - 1)));
      //---
      if((to < stop)){
        merge(a, from, mid, to);
      } else{
        merge(a, from, mid, stop);
      }
      i = (i + (m + m));
    }
    m = (m + m);
  }
}
