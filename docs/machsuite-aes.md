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
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
