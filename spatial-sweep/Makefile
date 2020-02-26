.PHONY: all clean

DATA:= data/input_matrix_a.csv data/input_matrix_b.csv data/resource_summary.csv

all: $(DATA)
	./graphs.py

$(DATA):
	./summarize.py

clean:
	rm *pdf
	rm $(DATA)
