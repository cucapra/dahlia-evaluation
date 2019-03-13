.PHONY: resume all extract clean

BENCHMARKS:=./machsuite-gemm-ncubed
JOBS:=jobs.txt
FAILED_EX:=failed-extract.txt

all:
	./scripts/batch.py $(BENCHMARKS) > $(JOBS)

resume:
	cat failure.txt | xargs ./scripts/batch.py

extract:
	./scripts/extract.py $(JOBS) > $(FAILED_EX)

clean:
	rm -rf $(JOBS)
