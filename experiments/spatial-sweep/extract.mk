.PHONY: phony

absolute.png: summary.csv
	jupyter nbconvert --execute analysis.ipynb

summary.csv: phony
	./summary-rpts.py
