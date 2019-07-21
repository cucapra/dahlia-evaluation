#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  TYPE (*orig_l)[64] = (TYPE (*)[64])&args->orig[0];
  TYPE (*sol_l)[64] = (TYPE (*)[64])&args->sol[0];
  TYPE (*filter_l)[3] = (TYPE (*)[3])&args->filter[0];
  stencil( orig_l, sol_l, filter_l );
}
