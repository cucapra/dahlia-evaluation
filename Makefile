.PHONY: resume all extract clean fuse summarize

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
	rewrite/machsuite-aes \
	rewrite/machsuite-bfs-bulk \
	rewrite/machsuite-bfs-queue \
	rewrite/machsuite-fft-strided \
	rewrite/machsuite-fft-transpose \
	rewrite/machsuite-gemm-blocked \
	rewrite/machsuite-gemm-ncubed \
	rewrite/machsuite-kmp \
	rewrite/machsuite-md-grid \
	rewrite/machsuite-md-knn \
	rewrite/machsuite-nw \
	rewrite/machsuite-sort-merge \
	rewrite/machsuite-sort-radix \
	rewrite/machsuite-spmv-crs \
	rewrite/machsuite-spmv-ellpack \
	rewrite/machsuite-stencil-stencil2d \
	rewrite/machsuite-stencil-stencil3d \
	rewrite/machsuite-viterbi

LAST_BATCH := last_batch.txt

all:
	./_scripts/batch.py $(BENCHMARKS) -E > $(LAST_BATCH)

resume-batch:
	cat $(FAILED_BATCH) | xargs
	./_scripts/batch.py _results/$(shell cat $(LAST_BATCH))/failure_batch.txt

extract:
	./_scripts/extract.py _results/$(shell cat $(LAST_BATCH))

resume-extract:
	./_scripts/extract.py _results/$(shell cat $(LAST_BATCH))/failure_extract.txt

summarize:
	./_scripts/summarize.py _results/$(shell cat $(LAST_BATCH))/results.json

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
