# ORIGINAL
Resides in original/sort/radix

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ss_sort` to `sort` in all three source files.

# BASELINE
Data zero copy pragma used for all 4 arguments.

# REWRITE
2D arrays are used. `init` uses two loops. `/` and `%` by 4 and 16 used to get dimension index in `hist` and `update`

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
