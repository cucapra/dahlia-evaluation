# ORIGINAL
Resides in original/sort/merge

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ms_mergesort` to `sort` in all three source files.

# BASELINE
Data zero copy pragma used for the argument.
Add tripcount to all 5 loops (only adding for `merge` or `sort` didn't work, but smaller configuration might).
1 max: 1024, min: 0, total: 11264, number: 2047, avg: 5
3 max: 2048, min: 0, total: 22528, number: 2047, avg: 11
max: 1024, min: 0, total: 2047, number: 11, avg: 186 
BRAM resource directives commented out because #35. It seems arguments cannot be allocated memory? `WARNING: [SYN 201-303] Cannot apply memory assignment of 'RAM_1P_BRAM' on 'a', which is not an array.`
Commenting out pipeline directives as they cannot be with variable bounds.
Loops in merge throw `WARNING: [SCHED 204-65] Unable to satisfy pipeline directive: Function contains subloop(s) not being unrolled.` because `WARNING: [XFORM 203-503] Cannot unroll loop 'merge_label1' (/seashell/buildbot/instance/jobs/uNa_G_ncr7w/code/sort.cpp:7) in function 'merge' completely: variable loop bound.`. 
Loop in sort throws `WARNING: [SCHED 204-67] Unable to satisfy pipeline directive: subfunction 'merge' is not pipelined.`. 

# REWRITE
Uses while loops for `for` loops as the bounds are runtime determined. Loop iterators step by factors in baseline, this is done manually since no `for` loop support in our language.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
