.PHONY: resume all extract clean

BENCHMARKS := baseline/machsuite-gemm-ncubed \
	baseline/machsuite-gemm-blocked \
	baseline/machsuite-stencil-stencil2d \
	baseline/machsuite-stencil-stencil3d
JOBS := jobs.txt
FAILED_BATCH := failure.txt
FAILED_EX := failed-extract.txt

all:
	./_scripts/batch.py $(BENCHMARKS) > $(JOBS)

resume-batch:
	cat $(FAILED_BATCH) | xargs ./_scripts/batch.py

extract:
	./_scripts/extract.py $(JOBS) > $(FAILED_EX)

resume-extract:
	cat $(FAILED_EX) | xargs ./_scripts/extract.py

clean:
	rm -rf $(JOBS)
