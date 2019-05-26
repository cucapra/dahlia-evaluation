# ORIGINAL
Resides in original/fft/strided

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for both arguments.
Add tripcount to inner loop for estimation.

# REWRITE
`bit<32>`s are replaced with `int`s.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
