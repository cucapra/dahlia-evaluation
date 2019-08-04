from __future__ import print_function

import sys

TCL_TMPL = """
set hls_prj benchmark.prj
open_project ${{hls_prj}} -reset

set_top {func}  ; # The name of the hardware function.
add_files {sources}  ; # HLS source files.

open_solution "solution1"
set_part {{xc7z020clg484-1}}
create_clock -period {clock_period}

# Source directives
source "{func}_dir"

# Actions we can take include csim_design, csynth_design, or cosim_design.
csynth_design

{synth_cmd}

exit
""".strip()

SYNTH_CMD = """
# This extra step runs Vivado synthesis.
export_design -flow syn
""".strip()


def gen_vhls_tcl(synthesize, func_name, source_files):
    return TCL_TMPL.format(
        synth_cmd=SYNTH_CMD if synthesize else '',
        sources=' '.join(source_files),
        func=func_name,
        clock_period=7,  # We're always using 7 ns for now.
    )


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) < 3 or args[0] not in ('hls', 'syn'):
        print('Usage: {} <hls|syn> <func> <source...>'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)
    print(gen_vhls_tcl(
        args[0] == 'syn',
        args[1],
        args[2:],
    ))
