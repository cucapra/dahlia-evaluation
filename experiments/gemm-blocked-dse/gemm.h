#include <ap_int.h>

//Define compute data type
#define TYPE ap_int<32>

//Specify row/column sizes
#define ROW_SIZE 128
#define N ROW_SIZE * ROW_SIZE
#define BLOCK_SIZE 8
#define NUMOFBLOCKS N/BLOCK_SIZE/BLOCK_SIZE

// Maximum range for randomly generated data.
#define RANGE 1000
