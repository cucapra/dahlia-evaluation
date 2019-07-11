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
Data copy pragma used for `pattern` to avoid #34.

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

`bit<32>` is manually changed to `int`; `bit<8>` is manually changed to `char`.

# OPTIMIZED BASELINE
No change made

# OPTIMIZED REWRITE
No change made
