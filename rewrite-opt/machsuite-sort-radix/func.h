#include "sort.h"

#pragma SDS data zero_copy(a[0:512][0:4], b[0:512][0:4], bucket[0:128][0:16], sum[0:128])
void sort(int a[NUMOFBLOCKS][ELEMENTSPERBLOCK], int b[NUMOFBLOCKS][ELEMENTSPERBLOCK], int bucket[SCAN_RADIX][SCAN_BLOCK], int sum[SCAN_RADIX]);
