#!/usr/bin/env python3

import pandas as pd
from benchmarking.plotting import plots

def make_plots():
    x_key = 'unroll'
    y_keys = [
        'runtime_avg',
        'lut_used',
        'bram_used',
        'dsp_used',
    ]
    df = pd.read_csv('./summary.csv').sort_values(by=x_key)

    plots.make_absolute_plots(
        df,
        x_key,
        y_keys,
        group_by = lambda _: 0,
        group_labels = None,
        x_label = 'Unrolling factor',
        fig_prefix = 'no-partition-unrolling')

if __name__ == '__main__':
    make_plots()
