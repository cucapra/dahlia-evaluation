# ORIGINAL
Resides in original/sort/merge

## Changes made to the original

# SOFTWARE
All `.c` files changed to `.cpp`.
Function name changed from `ms_mergesort` to `sort` in all three source files.

# BASELINE
Data zero copy pragma used for the argument.
Add tripcount to all 5 loops naively for estimation (only adding for `merge` or `sort` didn't work, but smaller configuration might).
BRAM resource directives commented out as they break the flow. #35

# REWRITE
After compiling the `.fuse` file
```
#ifdef __SDSCC__
#include "ap_int.h"
#else
template < int N >
using ap_int = int;
#endif
```
has to be added to the top of the generated cpp file for `ap_int` to be recognized.

Add interface pragmas and loop count pragmas which are not supported by code generation.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
