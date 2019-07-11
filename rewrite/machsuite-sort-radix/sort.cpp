#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif

void local_scan(ap_int<32> bucket[2048]) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> bucket_tmp1 = 0;
  ap_int<32> bucket_tmp2 = 0;
  for(int radix_id = 0; radix_id < 128; radix_id++) {
    for(int i = 1; i < 16; i++) {
      bucket_idx = ((radix_id * 16) + i);
      bucket_tmp1 = bucket[(bucket_idx - 1)];
      //---
      bucket_tmp2 = bucket[bucket_idx];
      //---
      bucket[bucket_idx] = (bucket_tmp1 + bucket_tmp2);
    }
  }
}
void sum_scan(ap_int<32> sum[128], ap_int<32> bucket[2048]) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> sum_tmp = 0;
  sum[0] = 0;
  //---
  for(int radix_id = 1; radix_id < 128; radix_id++) {
    bucket_idx = ((radix_id * 16) - 1);
    sum_tmp = sum[(radix_id - 1)];
    //---
    sum[radix_id] = (sum_tmp + bucket[bucket_idx]);
  }
}
void last_step_scan(ap_int<32> bucket[2048], ap_int<32> sum[128]) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> bucket_tmp = 0;
  for(int radix_id = 0; radix_id < 128; radix_id++) {
    for(int i = 0; i < 16; i++) {
      bucket_idx = ((radix_id * 16) + i);
      bucket_tmp = bucket[bucket_idx];
      //---
      bucket[bucket_idx] = (bucket_tmp + sum[radix_id]);
    }
  }
}
void init(ap_int<32> bucket[2048]) {
  
  for(int i = 0; i < 2048; i++) {
    bucket[i] = 0;
  }
}
void hist(ap_int<32> bucket[2048], ap_int<32> a[2048], ap_int<32> exp) {
  
  ap_int<32> elem_per_block = 4;
  ap_int<32> bucket_idx = 0;
  ap_int<32> bucket_tmp = 0;
  ap_int<32> a_idx = 0;
  for(int block_id = 0; block_id < 512; block_id++) {
    for(int i = 0; i < 4; i++) {
      a_idx = ((block_id * elem_per_block) + i);
      bucket_idx = ((((a[a_idx] >> exp) & 0x3) * 512) + (block_id + 1));
      bucket_tmp = bucket[bucket_idx];
      //---
      bucket[bucket_idx] = (bucket_tmp + 1);
    }
  }
}
void update(ap_int<32> b[2048], ap_int<32> bucket[2048], ap_int<32> a[2048], ap_int<32> exp) {
  
  ap_int<32> bucket_idx = 0;
  ap_int<32> elem_per_block = 4;
  ap_int<32> a_idx = 0;
  ap_int<32> bucket_temp = 0;
  for(int block_id = 0; block_id < 512; block_id++) {
    for(int i = 0; i < 4; i++) {
      bucket_idx = ((((a[((block_id * elem_per_block) + i)] >> exp) & 0x3) * 512) + block_id);
      //---
      a_idx = ((block_id * elem_per_block) + i);
      //---
      b[bucket[bucket_idx]] = a[a_idx];
      //---
      bucket_temp = bucket[bucket_idx];
      //---
      bucket[bucket_idx] = (bucket_temp + 1);
    }
  }
}
#pragma SDS data zero_copy(a[0:SIZE], b[0:SIZE], bucket[0:BUCKETSIZE], sum[0:SCAN_RADIX])
void sort(ap_int<32> a[2048], ap_int<32> b[2048], ap_int<32> bucket[2048], ap_int<32> sum[128]) {
  
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
