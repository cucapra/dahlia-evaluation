
data/summary.csv: data/results.json
	rm -f data/failure_extract.txt
	../../_scripts/summarize.py data/results.json -k key.json -c sda_est default -R

data/results.json: data/jobs.txt
	./get-files.py

full-synth/results.json: full-synth/jobs.txt
	../../_scripts/extract.py full-synth/ -c routed_util dse_template runtime

full-synth/summary.csv: full-synth/results.json
	../../_scripts/summarize.py full-synth/results.json -k key.json -c routed_util default
