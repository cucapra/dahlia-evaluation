#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  double (*m1_l)[64] = (double (*)[64])&args->m1[0];
  double (*m2_l)[64] = (double (*)[64])&args->m2[0];
  double (*prod_l)[64] = (double (*)[64])&args->prod[0];
  gemm( m1_l, m2_l, prod_l );
}
