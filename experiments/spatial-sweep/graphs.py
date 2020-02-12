#!/usr/bin/env python3

import pandas as pd
from benchmarking.plotting import plots

def make_plots():
    x_key = 'unroll'
    y_keys = [
        'lut_used',
        'reg_used',
        'dsp_used',
        'bram_tile_used'
    ]
    df = pd.read_csv('rpts/summary.csv').sort_values(by=x_key).reset_index()
    plots.make_absolute_plots(
        df,
        x_key,
        y_keys,
        group_by = lambda idx: int(16 % df.iloc[idx][x_key] == 0),
        group_labels = ['Unpredictable points', 'Predictable points'],
        group_markers = ['o', 'v', 'x'],
        x_label = 'Unrolling factor',
        fig_prefix = 'spatial-sweep',
        legend = 'lut_used'
    )

if __name__ == '__main__':
    make_plots()
