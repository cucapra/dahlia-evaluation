.PHONY: resume all extract clean fuse

BENCHMARKS := baseline/machsuite-aes \
	baseline/machsuite-backprop \
	baseline/machsuite-bfs-bulk \
	baseline/machsuite-bfs-queue \
	baseline/machsuite-fft-strided \
	baseline/machsuite-fft-transpose \
	baseline/machsuite-gemm-blocked \
	baseline/machsuite-gemm-ncubed \
	baseline/machsuite-kmp \
	baseline/machsuite-md-grid \
	baseline/machsuite-md-knn \
	baseline/machsuite-nw \
	baseline/machsuite-sort-merge \
	baseline/machsuite-sort-radix \
	baseline/machsuite-spmv-crs \
	baseline/machsuite-spmv-ellpack \
	baseline/machsuite-stencil-stencil2d \
	baseline/machsuite-stencil-stencil3d \
	baseline/machsuite-viterbi \
	rewrite/machsuite-bfs-bulk \
	rewrite/machsuite-fft-strided \
	rewrite/machsuite-gemm-ncubed \
	rewrite/machsuite-gemm-blocked \
	rewrite/machsuite-kmp \
	rewrite/machsuite-md-knn \
	rewrite/machsuite-sort-merge \
	rewrite/machsuite-sort-radix \
	rewrite/machsuite-spmv-crs \
	rewrite/machsuite-spmv-ellpack \
	rewrite/machsuite-stencil-stencil2d
JOBS := jobs.txt
LAST_BATCH := last_batch.txt
FAILED_BATCH := failure.txt
FAILED_EX := failed-extract.txt

all:
	./_scripts/batch.py $(BENCHMARKS) > $(LAST_BATCH)

resume-batch:
	cat $(FAILED_BATCH) | xargs ./_scripts/batch.py

extract:
	./_scripts/extract.py results/$(shell cat $(LAST_BATCH))

resume-extract:
	cat $(FAILED_EX) | xargs ./_scripts/extract.py

clean:
	rm -rf $(LAST_BATCH)

# Build Fuse source for each benchmark that has it.
fuse:
	@for bench in $(BENCHMARKS) ; do \
		if [ -f $$bench/*.fuse ] ; then \
			echo $$bench ; \
			make -C $$bench fuse ; \
		fi ; \
	done
