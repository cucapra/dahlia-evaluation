#include "spmv.h"

#pragma SDS data copy(nzval[0:494][0:10])
#pragma SDS data zero_copy(out[0:494])
void spmv(TYPE nzval[N][L], int32_t cols[N][L], TYPE vec[N], TYPE out[N]);
