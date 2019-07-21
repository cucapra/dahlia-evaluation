#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  int (*a_l)[4] = (int (*)[4])&args->a[0];
  int (*b_l)[4] = (int (*)[4])&args->b[0];
  int (*bucket_l)[16] = (int (*)[16])&args->bucket[0];
  sort( a_l, b_l, bucket_l, args->sum );
}
