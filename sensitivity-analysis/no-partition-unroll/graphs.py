#!/usr/bin/env python3

import pandas as pd
from benchmarking.plotting import plots

if __name__ == '__main__':
    x_key = 'unroll'
    df = pd.read_csv('./summary.csv').sort_values(by=x_key).reset_index()
    plots.make_absolute_plots(
        df,
        x_key = x_key,
        y_keys = ['runtime_avg', 'lut_used'],
        group_by = lambda x: 0,
        x_label = 'Unrolling factor (no partitioning)',
        fig_prefix = 'no-partition-unrolling',
        )
