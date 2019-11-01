.PHONY: get-jobs

extract-all:
	../_scripts/extract.py baseline/ -c routed_util
	../_scripts/extract.py rewrite/ -c routed_util

summarize-all:
	# Does not download runtime log. Remove -R to download runtime log.
	../_scripts/summarize.py baseline/results.json -c routed_util default -R
	../_scripts/summarize.py rewrite/results.json -c routed_util default -R
