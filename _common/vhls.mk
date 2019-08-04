# A common Makefile for compiling kernels *only* via Vivado HLS.
# As with `sds.mk`, set KERNEL, HW_SRCS, and possibly ESTIMATE.

ifeq ($(ESTIMATE),1)
VHLS_MODE := hls
else
VHLS_MODE := syn
endif

vhls: run_hls.tcl
	vivado_hls -f $<

run_hls.tcl:
	python gen_vhls_tcl.py $(VHLS_MODE) $(KERNEL) $(HW_SRCS) > $@
