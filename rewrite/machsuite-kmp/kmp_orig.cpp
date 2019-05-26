
void cpf(ap_int<8> pattern[4], ap_int<32> kmp_next[4]) {
  
  ap_int<32> k = 0;
  kmp_next[0] = 0;
  //---
  for(int q = 1; q < 4; q++) {
    ap_int<8> k_val = pattern[k];
    //---
    ap_int<8> q_val = pattern[q];
    //---
    while(((k > 0) && (k_val != q_val))) {
      k = kmp_next[q];
      k_val = pattern[k];
      //---
      q_val = pattern[q];
    }
    //---
    if((k_val == q_val)){
      k = (k + 1);
    } else{
      
    }
    kmp_next[q] = k;
  }
}
void kmp(ap_int<8> pattern[4], ap_int<8> input[32411], ap_int<32> kmp_next[4], ap_int<32> n_matches[1]) {
  
  n_matches[0] = 0;
  cpf(pattern, kmp_next);
  //---
  ap_int<32> q = 0;
  for(int i = 0; i < 32411; i++) {
    while(((q > 0) && (pattern[q] != input[i]))) {
      q = kmp_next[q];
    }
    //---
    if((pattern[q] == input[i])){
      q = (q + 1);
    } else{
      
    }
    if((q >= 4)){
      ap_int<32> temp = n_matches[0];
      //---
      n_matches[0] = (temp + 1);
      q = kmp_next[(q - 1)];
    } else{
      
    }
  }
}