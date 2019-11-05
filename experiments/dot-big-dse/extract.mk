
data/summary.csv: data/results.json
	rm -f data/failure_extract.txt
	../../_scripts/summarize.py data/results.json -k key.json -c sda_est default -R


data/results.json: data/jobs.txt
	./get-files.py
