KEYS=routed_util
SOURCES=absolute.png normalized.png

$(SOURCES): baseline/summary.csv rewrite/summary.csv analysis.ipynb
	jupyter nbconvert --execute analysis.ipynb

%/results.json: %/jobs.txt
	../_scripts/extract.py $* -c $(KEYS) runtime

%/summary.csv: %/results.json
	../_scripts/summarize.py $< -c $(KEYS) default
