#include "sort.h"

#pragma SDS data zero_copy(a[0:SIZE], b[0:SIZE], bucket[0:BUCKETSIZE], sum[0:SCAN_RADIX])
void sort(int a[SIZE], int b[SIZE], int bucket[BUCKETSIZE], int sum[SCAN_RADIX]);
