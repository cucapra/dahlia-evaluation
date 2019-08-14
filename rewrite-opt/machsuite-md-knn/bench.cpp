#include "func.h"

void run_benchmark( void *vargs ) {
  struct bench_args_t *args = (struct bench_args_t *)vargs;
  int32_t (*NL_l)[16] = (int32_t (*)[16])&args->NL[0];
  md( args->force_x, args->force_y, args->force_z,
      args->position_x, args->position_y, args->position_z,
      NL_l );
}
