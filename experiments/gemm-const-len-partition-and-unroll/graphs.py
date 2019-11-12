#!/usr/bin/env python3

import pandas as pd
from benchmarking.plotting import plots


def make_plots():
    x_key = 'partition'
    y_keys = [
        'runtime_avg',
        'lut_used',
        'bram_used',
        'dsp_used',
    ]
    df = pd.read_csv('./summary.csv').sort_values(by=x_key).reset_index()

    def group(idx):
        row = df.iloc[idx]
        if row.status == 'error':
            return 2
        return int(16 % df.iloc[idx][x_key] == 0)

    plots.make_absolute_plots(
        df,
        x_key,
        y_keys,
        # group_by = lambda row: 1 if 8 % row.partition == 0 else 0,
        group_by = group,
        group_labels = ['Unpredictable points', 'Predictable points', 'Incorrect hardware'],
        group_markers = ['o', 'o', 'x'],
        x_label = 'Partitioning and Unrolling factor',
        fig_prefix = 'const-len-partition-and-unroll')

if __name__ == '__main__':
    make_plots()
