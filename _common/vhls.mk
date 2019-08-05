# A common Makefile for compiling kernels *only* via Vivado HLS.
# As with `sds.mk`, set KERNEL, HW_SRCS, and possibly ESTIMATE.

CURLFLAGS := -F hwname=$(KERNEL)
CURLFLAGS += -F directives=$(DIRECTIVES)

ifeq ($(ESTIMATE),1)
VHLS_MODE := hls
CURLFLAGS += -F estimate=1
else
VHLS_MODE := syn
endif

vhls: run_hls.tcl
	vivado_hls -f $<

ifeq ($(DIRECTIVES),)
DIRARG := -
else
DIRARG := $(DIRECTIVES)
endif
run_hls.tcl:
	python gen_vhls_tcl.py $(VHLS_MODE) $(DIRARG) $(KERNEL) $(HW_SRCS) > $@

.PHONY: submit
BUILDBOT := http://gorgonzola.cs.cornell.edu:8000

submit:
	zip -r - . | curl -F file='@-;filename=code.zip' $(CURLFLAGS) -F make=1 \
		$(BUILDBOT)/jobs
