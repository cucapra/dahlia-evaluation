#include "gemm.h"

//#pragma SDS data copy(m1[0:row_size][0:col_size], m2[0:row_size][0:col_size], prod[0:row_size][0:col_size])
#pragma SDS data copy(m1[0:row_size][0:col_size])
void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]);
