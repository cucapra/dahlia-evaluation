#!/usr/bin/env python3

import re
import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from benchmarking.plotting import plots


def make_plots():
    """Generate resource graphs for post synthesis results against the unrolling
    factor.
    """
    x_key = 'unroll'
    y_keys = [
        'lut_used',
        'reg_used',
        'dsp_used',
        'bram_tile_used'
    ]
    df = pd.read_csv('data/resource_summary.csv').sort_values(by=x_key).reset_index()
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

def make_decisions_plot():
    """Generate a graph for the banking decisions spatial made for the two
    input matrices.
    """
    cl = sns.color_palette("colorblind",6)

    fig,ax1 = plt.subplots()
    loop_k = np.arange(16)+1
    marker = {
        'input_matrix_a':'s',
        'input_matrix_b':'+'
    }
    leg1 = ['input matrix a','input matrix b']
    #ax2 = ax1.twinx()
    ms = [8,11]
    for i, sram in enumerate(marker):
        df = pd.read_csv('data/' + sram + '.csv')
        bank = [ eval(re.sub(r"\s+", ",", n.strip()))[0] for n in df[' N']]
        ax1.plot(loop_k,bank, marker[sram], color=cl[i], ms = ms[i], mew=3)

    ax1.legend(leg1,loc=4, prop={'size':16})
    ax1.set_ylabel('Banking Decisions', fontsize = 18)
    ax1.set_xlabel('Unrolling Factor',fontsize = 18)
    plt.xticks(fontsize = 16)
    plt.yticks(fontsize = 16)
    fig.tight_layout()
    plt.savefig('spatial-sweep-decisions.pdf', dpi=200)

def make_norm_plot():
    """Normalize the resource counts against no unrolling.
    """
    PERF_RESOURCES = ['dsp_used', 'bram_tile_used','lut_used']

    df = pd.read_csv('data/resource_summary.csv')

    # Normalize resource usage against the no unrolling case.
    for res in PERF_RESOURCES:
        baseline = df[res][0]
        df[res + "_norm"] = df[res] / baseline

    fig = plt.figure()
    ax = fig.gca()
    loop_k = np.arange(16)+1
    maker = ['X','D','o']
    for i, res in enumerate(PERF_RESOURCES):
        y = df[res + '_norm']
        sns.lineplot(loop_k, y, linewidth=3, marker=maker[i], ms=10)

    plt.xticks(fontsize = 14)
    plt.yticks(fontsize = 14)
    ax.set_ylabel('Normalized Resource Usages',fontsize= 16)
    ax.set_xlabel('Unrolling Factor',fontsize = 16)

    PERF_RESOURCES_NAME = ['DSP used', 'BRAM used', 'LUT used']
    ax.legend(PERF_RESOURCES_NAME, prop={'size': 14})
    plt.savefig('spatial-sweep-normalized.pdf', bbox_inches='tight', dpi=200)

if __name__ == '__main__':
    make_plots()
    make_decisions_plot()
    make_norm_plot()
