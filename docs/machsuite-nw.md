# ORIGINAL
Resides in original/nw/nw

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `needwun` to `nw` in all three source files.

# BASELINE
Data zero copy pragma used for `M, ptr` arguments.
Add tripcount to all 8 loops naively for estimation (only adding for `trace` didn't work).
BRAM resource directives commented out as they break the flow. #35
Data copy pragma used for `SEQA` to avoid #34.

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
