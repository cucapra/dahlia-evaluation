#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  TYPE (*nzval_l)[10] = (TYPE (*)[10])&args->nzval[0];
  int32_t (*cols_l)[10] = (int32_t (*)[10])&args->cols[0];
  spmv( nzval_l, cols_l, args->vec, args->out );
}
