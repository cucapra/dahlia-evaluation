#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  spmv( args->nzval, args->cols, args->vec, args->out );
}
