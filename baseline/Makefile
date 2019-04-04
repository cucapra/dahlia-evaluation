BENCHMARKS=\
	machsuite-aes \
	machsuite-backprop \
	machsuite-bfs-bulk \
	machsuite-bfs-queue \
	machsuite-fft-strided \
	machsuite-fft-transpose \
	machsuite-gemm-ncubed \
	machsuite-gemm-blocked \
	machsuite-kmp \
	machsuite-md-knn \
	machsuite-md-grid \
	machsuite-nw \
	machsuite-sort-merge \
	machsuite-sort-radix \
	machsuite-spmv-crs \
	machsuite-spmv-ellpack \
	machsuite-stencil-stencil2d \
	machsuite-stencil-stencil3d \
	machsuite-viterbi

CMD =
ifdef ESTIMATE
CMD += ESTIMATE=1
endif

ifdef DIRECTIVE
CMD += DIRECTIVE=1
endif

.PHONY: build submit clean

build:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b; done )

submit:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b submit; done )

clean:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b clean || exit ; done )
