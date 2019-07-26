/*
Implementation based on http://www-igm.univ-mlv.fr/~lecroq/string/node8.html
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "support.h"

#define PATTERN_SIZE 4
#define STRING_SIZE (32411)

#pragma SDS data copy(pattern[0:PATTERN_SIZE])
#pragma SDS data zero_copy(kmpNext[0:PATTERN_SIZE], n_matches)
#pragma SDS data zero_copy(input[0:STRING_SIZE])
void kmp(char pattern[PATTERN_SIZE], char input[STRING_SIZE], int32_t kmpNext[PATTERN_SIZE], int32_t n_matches[1]);
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  char pattern[PATTERN_SIZE];
  char input[STRING_SIZE];
  int32_t kmpNext[PATTERN_SIZE];
  int32_t n_matches[1];
};
