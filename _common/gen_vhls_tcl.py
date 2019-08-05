from __future__ import print_function

import sys

TCL_TMPL = """
set hls_prj benchmark.prj
open_project ${{hls_prj}} -reset

set_top {top}  ; # The name of the hardware function.
add_files {sources}  ; # HLS source files.

open_solution "solution1"
set_part {{xc7z020clg484-1}}
create_clock -period {clock_period}

{dir_cmd}

# Actions we can take include csim_design, csynth_design, or cosim_design.
csynth_design

{synth_cmd}

exit
""".strip()

SYNTH_CMD = """
# This extra step runs Vivado synthesis.
export_design -flow syn
""".strip()

DIR_CMD = """
# Source directives
source "{}"
""".strip()

def gen_vhls_tcl(synthesize, directives, func_name, source_files):
    return TCL_TMPL.format(
        synth_cmd=SYNTH_CMD if synthesize else '',
        dir_cmd=DIR_CMD.format(directives),
        sources=' '.join(source_files),
        top=func_name,
        clock_period=7,  # We're always using 7 ns for now.
    )


if __name__ == '__main__':
    args = sys.argv[1:]
    if len(args) < 4 or args[0] not in ('hls', 'syn'):
        print('Usage: {} <hls|syn> <func> <source...>'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)
    print(gen_vhls_tcl(
        args[0] == 'syn',
        args[1],
        args[2],
        args[3:],
    ))
