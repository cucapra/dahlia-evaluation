.PHONY: resume all extract clean

BENCHMARKS := ./machsuite-gemm-ncubed
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
