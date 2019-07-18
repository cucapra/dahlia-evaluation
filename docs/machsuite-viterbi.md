# ORIGINAL
Resides in original/viterbi/viterbi

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for path argument.
Data copy pragma used for `obs` to avoid #34.

# REWRITE
`ap_int<8>` manually changed to `char` for software execution.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
