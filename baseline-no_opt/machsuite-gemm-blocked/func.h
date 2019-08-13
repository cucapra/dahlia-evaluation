#include "gemm.h"

#pragma SDS data copy(m1[0:N])
#pragma SDS data zero_copy(prod[0:N])
void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]);
