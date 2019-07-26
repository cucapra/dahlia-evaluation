# ORIGINAL
Resides in original/aes/aes

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `aes256_encrypt_ecb` to `aes` in all three source files.
original uses struct `aes256_context` to pass arguments to hardware, this is stripped away to the underlying three arrays in the c files.

# BASELINE
Data zero copy pragma used for all 4 arguments.

# REWRITE
Second loop changed to increment.
F call from aes_expandkey function taken out to avoid pass by pointer.
Defines inlined.
Hex computation written in decimal.
aes_roundkeys, and subBytes for loop used instead of while, and incrementing iterator.
return function will make rewrite lot easier to write.
Pass sbox array around instead of global.
Use offset variable to avoid passing part of k array by address. (Array split option to use only part of an array?)
in cpp, rj_xtime return values and function return; mix_column use return of function; add uint8_t and inttypes

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
