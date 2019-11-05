#!/usr/bin/env python3

import re
import csv
import json

import pandas as pd
import csv
import numpy as np

from benchmarking.parsing.rpt import RPTParser
from benchmarking.summary import extracting as ex


# Generate summary csv
with open('summary.csv', 'w') as csvfile:
    # Get fieldnames from the first file
    fieldnames = list(ex.sds_report("rpts/par_utilization_1.rpt").keys()) + ['unroll']
    summary = csv.DictWriter(csvfile, delimiter=',', fieldnames=fieldnames)
    summary.writeheader()

    # Get remaining rows
    for i in range(1,17):
        row = ex.sds_report("rpts/par_utilization_{}.rpt".format(i))
        row['unroll'] = i
        summary.writerow(row)
