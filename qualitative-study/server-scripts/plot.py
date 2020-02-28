#!/usr/bin/env python3

import numpy as np
import pandas as pd
import os
import sys

from benchmarking.plotting import plots

SCALE_X = {"scale_fun": lambda x: float(x / 100), "scale_label": "Hundreds of cycles"}

RESOURCES = ['hls_bram', 'hls_dsp', 'hls_ff', 'hls_lut', 'avg_latency']

def find_pareto(costs):
    """
    Find the pareto-efficient points
    :param costs: An (n_points, n_costs) array
    :return: A (n_points, ) boolean array, indicating whether each point is Pareto efficient
    """
    is_efficient = np.ones(costs.shape[0], dtype = bool)
    for i, c in enumerate(costs):
        (before, after) = costs[:i], costs[i+1:]
        # All points such that there is no point which has at least one objective minimized better and other objectives at least equal.
        is_efficient[i] = np.all(np.logical_not(np.logical_and(np.all(before <= c, axis=1), np.any(before < c, axis=1)))) and \
                          np.all(np.logical_not(np.logical_and(np.all(after <= c, axis=1), np.any(after < c, axis=1))))
    return is_efficient

def plot_stencil2d():
    df = pd.read_csv('machsuite-stencil-stencil2d-inner-data/summary.csv')
    print(df.info())
    df['avg_latency'] = ((df['hls_lat_min'] + df['hls_lat_max']) / 2)
    res = 'ur_loop2'

    opts = find_pareto(df[RESOURCES].to_numpy())
    pareto = df.iloc[opts]

    print(f'[Qualitative Study] Accepted Pareto points for stencil2d: {len(pareto)}/{len(df)}')

    def group(idx):
        return df.iloc[idx][res]

    plots.make_qual_plot(
        pareto,
        x_data = {'key': 'avg_latency', 'label': 'Average Latency'},
        y_data = {'key': 'hls_lut', 'label': 'LUTs Used'},
        group_by = group,
        legend="Inner Unroll",
        fig_prefix = "data-collect-stencil-inner-2d-inner-unroll",
        scale_x = SCALE_X,
        fig_dir = './',
        dpi=300,
    )

def plot_md_knn():
    df = pd.read_csv('machsuite-md-knn-data/summary.csv')
    df['avg_latency'] = ((df['hls_lat_min'] + df['hls_lat_max']) / 2)
    df = df[df.avg_latency < 100000].reset_index()
    res = 'ur_loop1'

    opts = find_pareto(df[RESOURCES].to_numpy())
    pareto = df.iloc[opts]

    print(f'[Qualitative Study] Accepted Pareto points for md-knn: {len(pareto)}/{len(df)}')

    def group(idx):
        return df.iloc[idx][res]

    plots.make_qual_plot(
        pareto,
        x_data = {'key': 'avg_latency', 'label': 'Average Latency'},
        y_data = {'key': 'hls_lut', 'label': 'LUTs Used'},
        group_by = group,
        legend='Outer Unroll',
        fig_prefix = "data-collect-md-knn-outer-unroll",
        scale_x = SCALE_X,
        fig_dir = './',
        dpi=300,
    )

def plot_md_grid():
    df = pd.read_csv('machsuite-md-grid-data/summary.csv')
    df['avg_latency'] = ((df['hls_lat_min'] + df['hls_lat_max']) / 2)

    res = 'B0Y_UR'
    opts = find_pareto(df[RESOURCES].to_numpy())
    pareto = df.iloc[opts]

    print(f'[Qualitative Study] Accepted Pareto points for md-grid: {len(pareto)}/{len(df)}')

    def group(idx):
        return df.iloc[idx][res]

    plots.make_qual_plot(
        pareto,
        x_data = {'key': 'avg_latency', 'label': 'Average Latency'},
        y_data = {'key': 'hls_lut', 'label': 'LUTs Used'},
        group_by = group,
        legend="Middle Unroll",
        fig_prefix = "data-collect-md-grid-middle-unroll",
        scale_x = SCALE_X,
        fig_dir = './',
        dpi=300,
    )

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Missing benchmark name.')
        sys.exit(1)

    if sys.argv[1] == 'machsuite-stencil-stencil2d-inner':
        plot_stencil2d()
    elif sys.argv[1] == 'machsuite-md-knn':
        plot_md_knn()
    elif sys.argv[1] == 'machsuite-md-grid':
        plot_md_grid()
    else:
        print('Unknown benchmark {sys.argv[1]}. Should be one of machsuite-stencil-stencil2d-inner, machsuite-md-grid, machsuite-md-knn')
