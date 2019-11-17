#!/usr/bin/env python3

import pandas as pd
from benchmarking.plotting import plots

if __name__ == '__main__':
    x_key = 'partition'
    df = pd.read_csv('./summary.csv').sort_values(by=x_key).reset_index()
    plots.make_sec2_plot(
        df=df,
        x_key = x_key,
        x_label = 'Partitioning and Unrolling factor',
        fig_prefix = 'const-len-partition-and-unroll')
