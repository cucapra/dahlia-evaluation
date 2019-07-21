#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  double (*transition_l)[64] = (double (*)[64])&args->transition[0];
  double (*emission_l)[64] = (double (*)[64])&args->emission[0];
  viterbi( args->obs, args->init, transition_l, emission_l, args->path );
}
