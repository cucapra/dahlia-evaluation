// Avoid using `ap_int` in "software" compilation.
#ifdef __SDSCC__
#include "ap_int.h"
#else
template <int N> using ap_int = int;
template <int N> using ap_uint = unsigned int;
#endif

void local_scan(
ap_int<32> bucket[128][16]) {
  
  ap_int<32> bucket_tmp1 = 0;
  ap_int<32> bucket_tmp2 = 0;
  for(int radix_id = 0; radix_id < 128; radix_id++) {
    for(int i = 1; i < 16; i++) {
      bucket_tmp1 = bucket[radix_id][(i - 1)];
      //---
      bucket_tmp2 = bucket[radix_id][i];
      //---
      bucket[radix_id][i] = (bucket_tmp1 + bucket_tmp2);
    }
  }
}
void sum_scan(
ap_int<32> sum[128], 
ap_int<32> bucket[128][16]) {
  
  ap_int<32> sum_tmp = 0;
  sum[0] = 0;
  //---
  for(int radix_id = 0; radix_id < 127; radix_id++) {
    sum_tmp = sum[radix_id];
    //---
    sum[(radix_id + 1)] = (sum_tmp + bucket[radix_id][15]);
  }
}
void last_step_scan(
ap_int<32> bucket[128][16], 
ap_int<32> sum[128]) {
  
  ap_int<32> bucket_tmp = 0;
  for(int radix_id = 0; radix_id < 128; radix_id++) {
    for(int i = 0; i < 16; i++) {
      bucket_tmp = bucket[radix_id][i];
      //---
      bucket[radix_id][i] = (bucket_tmp + sum[radix_id]);
    }
  }
}
void init(
ap_int<32> bucket[128][16]) {
  
  for(int i = 0; i < 128; i++) {
    for(int j = 0; j < 16; j++) {
      bucket[i][j] = 0;
    }
  }
}
void hist(
ap_int<32> bucket[128][16], 
ap_int<32> a[512][4], ap_int<32> exp) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> bucket_tmp = 0;
  for(int block_id = 0; block_id < 512; block_id++) {
    for(int i = 0; i < 4; i++) {
      bucket_idx = ((((a[block_id][i] >> exp) & 0x3) * 512) + (block_id + 1));
      bucket_tmp = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
      //---
      bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp + 1);
    }
  }
}
void update(
ap_int<32> b[512][4], 
ap_int<32> bucket[128][16], 
ap_int<32> a[512][4], ap_int<32> exp) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> elem_per_block = 4;
  ap_int<32> a_idx = 0;
  ap_int<32> bucket_tmp = 0;
  for(int block_id = 0; block_id < 512; block_id++) {
    for(int i = 0; i < 4; i++) {
      bucket_idx = ((((a[block_id][i] >> exp) & 0x3) * 512) + block_id);
      bucket_tmp = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
      //---
      b[(bucket_tmp / 4)][(bucket_tmp % 4)] = a[block_id][i];
      bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp + 1);
    }
  }
}
#pragma SDS data zero_copy(a[0:512][0:4], b[0:512][0:4], bucket[0:128][0:16], sum[0:128])
void sort(
ap_int<32> a[512][4], 
ap_int<32> b[512][4], 
ap_int<32> bucket[128][16], 
ap_int<32> sum[128]) {
  
  ap_int<1> valid_buffer = 0;
  ap_int<1> buffer_a = 0;
  ap_int<1> buffer_b = 1;
  ap_int<1> temp_valid_buffer = 0;
  for(int exp = 0; exp < 16; exp++) {
    init(bucket);
    //---
    if((valid_buffer == buffer_a)){
      hist(bucket, a, (exp << 1));
    } else{
      hist(bucket, b, (exp << 1));
    }
    //---
    local_scan(bucket);
    //---
    sum_scan(sum, bucket);
    //---
    last_step_scan(bucket, sum);
    //---
    if((valid_buffer == buffer_a)){
      update(b, bucket, a, (exp << 1));
      temp_valid_buffer = buffer_b;
    } else{
      update(a, bucket, b, (exp << 1));
      temp_valid_buffer = buffer_a;
    }
    //---
    valid_buffer = temp_valid_buffer;
  }
}