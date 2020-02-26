#!/usr/bin/env python3

import csv
import codecs
from benchmarking.summary import extracting as ex
from banking_parser import BankingDecisionParser

RES_BASE = "data/resource-reports/"
# Generate summary csv for resource RPTs.
with open('data/resource_summary.csv', 'w') as csvfile:
    # Get fieldnames from the first file
    fieldnames = list(ex.sds_report(RES_BASE + "par_utilization_1.rpt").keys()) + ['unroll']
    summary = csv.DictWriter(csvfile, delimiter=',', fieldnames=fieldnames)
    summary.writeheader()

    # Get remaining rows
    for i in range(1,17):
        row = ex.sds_report(RES_BASE + "par_utilization_{}.rpt".format(i))
        row['unroll'] = i
        summary.writerow(row)


# Summarize the banking decisions.
for g in range(1,17):
    decision = f"data/banking-decisions/decisions_{g}.html"
    f=codecs.open(decision, 'r')
    content = f.read()
    # instantiate the parser and fed it some HTML
    parser = BankingDecisionParser(g)
    parser.feed(content)
