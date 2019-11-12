// git.status = clean, build.date = Sat Nov 09 13:00:37 EST 2019, git.hash = 75aa1ab
#include <ap_int.h>
extern "C" {
  void sort(ap_int<32> a[512][4], ap_int<32> b[512][4], ap_int<32> bucket[128][16], ap_int<32> sum[128]) {
    #pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=a bundle=control
    #pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=b bundle=control
    #pragma HLS INTERFACE m_axi port=bucket offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=bucket bundle=control
    #pragma HLS INTERFACE m_axi port=sum offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=sum bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    ap_int<1> valid_buffer = 0;
    ap_int<1> buffer_a = 0;
    ap_int<1> buffer_b = 1;
    ap_int<1> temp_valid_buffer = 0;
    for(int exp = 0; exp < 16; exp++) {
      for(int i = 0; i < 128; i++) {
        for(int j = 0; j < 16; j++) {
          bucket[i][j] = 0;
        }
      }
      //---
      if ((valid_buffer == buffer_a)) {
        ap_int<32> bucket_idx = 0;
        ap_int<32> bucket_tmp = 0;
        for(int block_id = 0; block_id < 512; block_id++) {
          for(int i = 0; i < 4; i++) {
            bucket_idx = (((((a[block_id][i] >> (exp << 1)) & 0x3) * 512) + block_id) + 1);
            bucket_tmp = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
            //---
            bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp + 1);
          }
        }
      } else {
        ap_int<32> bucket_idx = 0;
        ap_int<32> bucket_tmp = 0;
        for(int block_id = 0; block_id < 512; block_id++) {
          for(int i = 0; i < 4; i++) {
            bucket_idx = (((((b[block_id][i] >> (exp << 1)) & 0x3) * 512) + block_id) + 1);
            bucket_tmp = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
            //---
            bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp + 1);
          }
        }
      }
      //---
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
      //---
      ap_int<32> sum_tmp = 0;
      sum[0] = 0;
      //---
      for(int radix_id = 0; radix_id < 127; radix_id++) {
        sum_tmp = sum[radix_id];
        //---
        sum[(radix_id + 1)] = (sum_tmp + bucket[radix_id][15]);
      }
      //---
      ap_int<32> bucket_tmp = 0;
      for(int radix_id = 0; radix_id < 128; radix_id++) {
        for(int i = 0; i < 16; i++) {
          bucket_tmp = bucket[radix_id][i];
          //---
          bucket[radix_id][i] = (bucket_tmp + sum[radix_id]);
        }
      }
      //---
      if ((valid_buffer == buffer_a)) {
        ap_int<32> bucket_idx = 0;
        ap_int<32> elem_per_block = 4;
        ap_int<32> a_idx = 0;
        ap_int<32> bucket_tmp_10 = 0;
        for(int block_id = 0; block_id < 512; block_id++) {
          for(int i = 0; i < 4; i++) {
            bucket_idx = ((((a[block_id][i] >> (exp << 1)) & 0x3) * 512) + block_id);
            bucket_tmp_10 = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
            //---
            b[(bucket_tmp_10 / 4)][(bucket_tmp_10 % 4)] = a[block_id][i];
            bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp_10 + 1);
          }
        }
        temp_valid_buffer = buffer_b;
      } else {
        ap_int<32> bucket_idx = 0;
        ap_int<32> elem_per_block = 4;
        ap_int<32> a_idx = 0;
        ap_int<32> bucket_tmp_11 = 0;
        for(int block_id = 0; block_id < 512; block_id++) {
          for(int i = 0; i < 4; i++) {
            bucket_idx = ((((b[block_id][i] >> (exp << 1)) & 0x3) * 512) + block_id);
            bucket_tmp_11 = bucket[(bucket_idx / 16)][(bucket_idx % 16)];
            //---
            a[(bucket_tmp_11 / 4)][(bucket_tmp_11 % 4)] = b[block_id][i];
            bucket[(bucket_idx / 16)][(bucket_idx % 16)] = (bucket_tmp_11 + 1);
          }
        }
        temp_valid_buffer = buffer_a;
      }
      //---
      valid_buffer = temp_valid_buffer;
    }
  }
}