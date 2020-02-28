.PHONY: start-job summarize-data

BENCHS:= qualitative-study/machsuite-md-grid \
	qualitative-study/machsuite-md-knn \
	qualitative-study/machsuite-stencil-stencil2d-inner

ifeq (,$(filter $(BENCH),$(BENCHS)))
$(error "Invalid BENCH: $(MODE). Must be one of: $(BENCHS)")
endif

start-job: $(notdir $(BENCH))-accepted
	./_scripts/run-data-collection.sh $(BENCH)

summarize-data: $(notdir $(BENCH))-data/summary.csv

# Configuration for md-grid
machsuite-md-grid-data/summary.csv: machsuite-md-grid-data/results.json
	./_scripts/summarize.py $< -R -k qualitative-study/server-scripts/md-grid-key.json -c hls default

machsuite-md-grid-data/results.csv: machsuite-md-grid-data/jobs.txt
	./_scripts/extract.py $(dir $<) -c hls dse_template

# Configuration for md-knn
machsuite-md-knn-data/summary.csv: machsuite-md-knn-data/results.json
	./_scripts/summarize.py $< -R -k qualitative-study/server-scripts/md-knn-key.json -c hls default

machsuite-md-knn-data/results.csv: machsuite-md-knn-data/jobs.txt
	./_scripts/extract.py $(dir $<) -c hls dse_template

# Configuration for stencil2d
machsuite-stencil-stencil2d-inner-data/summary.csv: machsuite-stencil-stencil2d-inner-data/results.json
	./_scripts/summarize.py $< -R -k qualitative-study/server-scripts/stencil2d-inner-key.json -c hls default

machsuite-stencil-stencil2d-inner-data/results.json: machsuite-stencil-stencil2d-inner-data/jobs.txt
	./_scripts/extract.py $(dir $<) -c hls dse_template
