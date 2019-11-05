#!/usr/bin/env python3

import re
import csv
import json

from rpt import RPTParser
import pandas as pd
import csv
import numpy as np
# from matplotlib import pyplot as plt

#PERF_STATS = ['used', 'total']

def _find_row(table, key, value):
    """Given a list of dicts in `table`, find the dict whose value for
    `key` contains `value`.
    """
    for row in table:
        if value in row[key]:
            return row
    raise ValueError("row not found")

def spatial_report(filepath):
    parser = RPTParser(filepath)

    keys = [ 'Used', 'Available' ]

    # Various tables
    logic_table = parser.get_table(
        re.compile(r'1\. Slice Logic'), 2)
    mem_table = parser.get_table(
        re.compile(r'\. Memory'), 2)
    dsp_table = parser.get_table(
        re.compile(r'\. DSP'), 2)

    key_map = {
        'lut'         : _find_row(logic_table, 'Site Type', 'LUTs'),
        'lut_logic'   : _find_row(logic_table, 'Site Type', 'Logic'),
        'lut_mem'     : _find_row(logic_table, 'Site Type', 'Memory'),
        'reg'         : _find_row(logic_table, 'Site Type', 'Registers'),
        'reg_ff'      : _find_row(logic_table, 'Site Type', 'Flip Flop'),
        'reg_latch'   : _find_row(logic_table, 'Site Type', 'Latch'),
        'bram_tile'   : _find_row(mem_table, 'Site Type', 'Block RAM Tile'),
        'bram36'      : _find_row(mem_table, 'Site Type', 'RAMB36/'),
        'bram18'      : _find_row(mem_table, 'Site Type', 'RAMB18'),
        'dsp'         : _find_row(dsp_table, 'Site Type', 'DSPs'),
    }

    return {
        res + "_" + typ.lower() : float(table[typ]) for (res, table) in key_map.items() for typ in keys
    }


# Generate summary csv
with open('summary.csv', 'w') as csvfile:
    fieldnames = list(spatial_report("rpts/par_utilization_1.rpt").keys()) + ['unroll']
    summary = csv.DictWriter(csvfile, delimiter=',', fieldnames=fieldnames)
    summary.writeheader()
    for i in range(1,17):
        row = spatial_report("rpts/par_utilization_{}.rpt".format(i))
        row['unroll'] = i
        summary.writerow(row)
