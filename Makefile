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
	baseline-opt/machsuite-aes \
	baseline-opt/machsuite-backprop \
	baseline-opt/machsuite-bfs-bulk \
	baseline-opt/machsuite-bfs-queue \
	baseline-opt/machsuite-fft-strided \
	baseline-opt/machsuite-fft-transpose \
	baseline-opt/machsuite-gemm-blocked \
	baseline-opt/machsuite-gemm-ncubed \
	baseline-opt/machsuite-kmp \
	baseline-opt/machsuite-md-grid \
	baseline-opt/machsuite-md-knn \
	baseline-opt/machsuite-nw \
	baseline-opt/machsuite-sort-merge \
	baseline-opt/machsuite-sort-radix \
	baseline-opt/machsuite-spmv-crs \
	baseline-opt/machsuite-spmv-ellpack \
	baseline-opt/machsuite-stencil-stencil2d \
	baseline-opt/machsuite-stencil-stencil3d \
	baseline-opt/machsuite-viterbi \
	baseline-no_opt/machsuite-aes \
	baseline-no_opt/machsuite-backprop \
	baseline-no_opt/machsuite-bfs-bulk \
	baseline-no_opt/machsuite-bfs-queue \
	baseline-no_opt/machsuite-fft-strided \
	baseline-no_opt/machsuite-fft-transpose \
	baseline-no_opt/machsuite-gemm-blocked \
	baseline-no_opt/machsuite-gemm-ncubed \
	baseline-no_opt/machsuite-kmp \
	baseline-no_opt/machsuite-md-grid \
	baseline-no_opt/machsuite-md-knn \
	baseline-no_opt/machsuite-nw \
	baseline-no_opt/machsuite-sort-merge \
	baseline-no_opt/machsuite-sort-radix \
	baseline-no_opt/machsuite-spmv-crs \
	baseline-no_opt/machsuite-spmv-ellpack \
	baseline-no_opt/machsuite-stencil-stencil2d \
	baseline-no_opt/machsuite-stencil-stencil3d \
	baseline-no_opt/machsuite-viterbi \
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

plot:
	./_scripts/plots.py _results/$(shell cat $(LAST_BATCH))/results.json

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
