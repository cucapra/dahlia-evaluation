#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  sort( args->a, args->b, args->bucket, args->sum );
}
