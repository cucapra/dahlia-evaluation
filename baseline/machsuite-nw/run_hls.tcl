set hls_prj benchmark.prj
open_project ${hls_prj} -reset

set_top nw  ; # The name of the "top" hardware function.
add_files nw.cpp  ; # HLS source files.

open_solution "solution1"
set_part {xc7z020clg484-1}
create_clock -period 7

# Actions we can take include csim_design, csynth_design, or cosim_design.
csynth_design

# This extra step runs Vivado synthesis.
export_design -flow syn

exit
