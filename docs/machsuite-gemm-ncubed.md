# ORIGINAL
Resides in original/gemm/ncubed

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.

# BASELINE
Data copy pragma used for `m1` to avoid #34.

# REWRITE
No change made

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
## DSE
unroll both loops by 8                        - BRAMs overun  - bYhsKOCfbkE
unroll inner by 8 and middle by 4             - timing fail   - tgBnmVmbh1o
unroll inner by 8                             - timing fail   - LiaW05L3FJ8
unroll inner completely                       - timing fail   - HhA2w3DyOp4
unroll inner by 4 and middle by 8 outer by 4  - BRAM overun   - r9WfqBTrtyA
### Current best
unroll inner by 4 and middle by 8            - fSyh66HAlW8

unroll inner by 4 and middle by 16           - nWusTY_NKs8
unroll inner by 4 and middle by 4            - iArPVt-vrfc
unroll inner by 4 and middle by 2            - WpsQvZn3-6Y
unroll inner by 4 and middle by 4 outer by 4 - oatbvRc_Xhs
unroll inner by 4                            - 5QMi81s0u6A
unroll inner by 2                            - mZ7V1AsG9cM

