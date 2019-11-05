
OBJS := absolute.png normalized_resources.png

$(OBJS): summary.csv analysis.ipynb
	jupyter nbconvert --execute analysis.ipynb

summary.csv: results.json
	../../_scripts/summarize.py -k key.json results.json

results.json: jobs.txt
	# To collect hls reports, uncomment
	# ../../_scripts/extract.py ./ -c routed_util dse_template hls
	../../_scripts/extract.py ./ -c routed_util dse_template
