/*
Implementation based on algorithm described in:
The cache performance and optimizations of blocked algorithms
M. D. Lam, E. E. Rothberg, and M. E. Wolf
ASPLOS 1991
*/

#include <stdio.h>
#include <stdlib.h>
#include "support.h"

//Data Type
#define TYPE double

//Algorithm Parameters
#define row_size 64
#define col_size 64
#define N row_size*col_size
#define block_size 8
#define NUMOFBLOCKS N/block_size/block_size

//Define the input range to operate over
#define MIN 0.
#define MAX 1.0

//Set number of iterations to execute
#define MAX_ITERATION 1

#pragma SDS data copy(m1[0:row_size][0:col_size])
#pragma SDS data zero_copy(prod[0:row_size][0:col_size])
void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]);
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  TYPE m1[N];
  TYPE m2[N];
  TYPE prod[N];
};
