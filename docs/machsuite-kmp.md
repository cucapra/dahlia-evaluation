# ORIGINAL
Resides in original/kmp/kmp

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for `kmpNext, n_matches`.
Add tripcount to `k1, k2` and `c1, c2` for estimation.
BRAM resource directives commented out as they break the flow. #35
Partition directives commented  to appease #44.
Data zero copy pragma used for `input` to avoid #36.

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
