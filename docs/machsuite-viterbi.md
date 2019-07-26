# ORIGINAL
Resides in original/viterbi/viterbi

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for path argument.
Data copy pragma used for `obs` to avoid #34.
Remove return as it is not used.

# REWRITE
`ap_int<8>` manually changed to `uint8_t` for software execution, `char` didn't work after update. Using 2D arrays for transition and emission. While loop used for decrementing for loop. Manually added type redefines.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
