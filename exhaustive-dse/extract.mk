.PHONY: plots

plots: data/summary.csv data/dahlia-points.csv
	jupyter nbconvert --execute analysis.ipynb
