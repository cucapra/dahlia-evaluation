# ORIGINAL
Resides in original/gemm/blocked

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `bbgemm` to `gemm` in all three source files.

# BASELINE
Data copy pragma used for `m1` to avoid #34.

# REWRITE
2D arrays are used. Tile loop iterator is stepping by 1 and multiplied by tile factor at computation because our language doesn't support it, which makes this version use more multipliers. Loop order changed to allow combine.

# OPTIMIZED BASELINE
No change made
'
# OPTIMIZED REWRITE
## DSE
split views in jj and kk                     - 
`[Type error] [8.7] Cannot create split view m1_v: split factor 1 does not divide banking factor 1 in dimension 64 for m1.i`
`        split m1_v = m1[by 1][by 8];`
basic access unroll k                        - 
`[Type error] [13.22] Dynamic access of array 'm1' requires unbanked dimension. Actual banking factor: 8. Use a shrink view to create unbanked array.`
`        let temp_x = m1[i][kk * 8 + k];`
aligned view unroll k by 8 j by 8            - YYpGXdsvL1A - BRAM overun
### Current best
aligned view jj(16) unroll k by 8 j(4) by 4  - iOsIXO3NKtU
aligned view unroll k by 8                   - dmuM2chxUG4
no change                                    - R8zBqbJdlXY
aligned view no banking                      - 0BrH77HvJhY
