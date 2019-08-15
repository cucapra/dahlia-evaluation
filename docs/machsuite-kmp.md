# ORIGINAL
Resides in original/kmp/kmp

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data zero copy pragma used for `kmpNext, n_matches`.
Add tripcount to `k1, k2` and `c1, c2` for estimation.
CPF c2 max: 0, min: 0, total: 0, number: 3, avg: 0
kmp k2 max: 1, min: 0, total: 438, number: 32411, avg: 0  
BRAM resource directives commented out as they break the flow. #35
Partition directives commented  to appease #44.
Data zero copy pragma used for `input` to avoid #36.
Data copy pragma used for `pattern` to avoid #34.

# REWRITE
For `bit<8>`, `ap_int<8>` is manually changed to `char` to avoid interface conflicts only in software.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
