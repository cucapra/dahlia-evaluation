#include "spmv.h"

#pragma SDS data copy(nzval[0:N*L])
#pragma SDS data zero_copy(out[0:N])
void spmv(TYPE nzval[N*L], int32_t cols[N*L], TYPE vec[N], TYPE out[N]);
