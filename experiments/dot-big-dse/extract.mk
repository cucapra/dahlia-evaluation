
full-synth/results.json: full-synth/jobs.txt
	../../_scripts/extract.py full-synth/ -c routed_util dse_template runtime

full-synth/summary.csv: full-synth/results.json
	../../_scripts/summarize.py full-synth/results.json -k key.json -c routed_util default

%/summary.csv: %/results.json
	../../_scripts/summarize.py $< -k key.json -c sda_est default -R

%/results.json: %/jobs.txt
	./get-files.py $*

