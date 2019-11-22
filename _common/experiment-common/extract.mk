.PHONY: graphs clean

graphs: summary.csv graphs.py
	./graphs.py

summary.csv: results.json key.json
	../../_scripts/summarize.py -k key.json results.json -c routed_util

results.json: jobs.txt
	../../_scripts/extract.py ./ -c dse_template routed_util runtime

clean:
	rm -rf summary.csv results.json *.pdf *.png raw
