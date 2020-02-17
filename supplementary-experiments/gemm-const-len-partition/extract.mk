misaligned-partition-absolute-lut-used.pdf misaligned-partition-absolute-runtime-avg.pdf: summary.csv
	jupyter nbconvert --execute analysis.ipynb

summary.csv: results.json
	../../_scripts/summarize.py -k key.json results.json -c routed_util


results.json: jobs.txt
	../../_scripts/extract.py ./ -c dse_template routed_util runtime
