#include <stdio.h>
#include <stdlib.h>
#include "support.h"

#define ALEN 128
#define BLEN 128

#pragma SDS data copy(SEQA[0:ALEN])
#pragma SDS data zero_copy(M[0:16641], ptr[0:16641])  
void nw(char SEQA[ALEN], char SEQB[BLEN],
             char alignedA[ALEN+BLEN], char alignedB[ALEN+BLEN],
             int M[(ALEN+1)*(BLEN+1)], char ptr[(ALEN+1)*(BLEN+1)]);
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  char seqA[ALEN];
  char seqB[BLEN];
  char alignedA[ALEN+BLEN];
  char alignedB[ALEN+BLEN];
  int M[(ALEN+1)*(BLEN+1)];
  char ptr[(ALEN+1)*(BLEN+1)];
};
