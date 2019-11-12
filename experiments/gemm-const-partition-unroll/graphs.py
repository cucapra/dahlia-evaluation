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
    df = pd.read_csv('./summary.csv').sort_values(by=x_key).reset_index()

    # Drop data points beyond 16. These were there to measure extrapolation
    # of DSPs
    df = df[df[x_key] < 17]

    plots.make_absolute_plots(
        df,
        x_key,
        y_keys,
        # group_by = lambda row: 1 if 8 % row.partition == 0 else 0,
        group_by = lambda idx: int(16 % df.iloc[idx][x_key] == 0),
        group_labels = ['Unpredictable points', 'Predictable points'],
        x_label = 'Unrolling factor (partitionin = 8)',
        fig_prefix = 'const-partition-unroll')

if __name__ == '__main__':
    make_plots()
