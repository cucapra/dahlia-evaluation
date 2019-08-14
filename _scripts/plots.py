#!/usr/bin/env python3

import json
import sys
import os
import numpy as np
import matplotlib.pyplot as plt
from summarize import summarize_one, find_missing
from plotting import plot_n_fields, subplot_n_fields

# Hope to use these to create resource arrays
RESOURCE_FIELDS = [
    'hls_lut',
    'hls_ff',
    'hls_bram',
    'hls_dsp',
    'est_lut',
    'est_ff',
    'est_bram',
    'est_dsp',
    'full_lut',
    'full_ff',
    'full_bram',
    'full_dsp',
]

# Hope to use these to create latency arrays
LATENCY_FIELDS = [
    'hls_lat_min',
    'hls_lat_max',
]

# Select benches to remove for analysis
MARKED_BENCHES = [
    'backprop',
]

# Select benchmarks to compare for analysis
SELECT_BENCHES = [
    'baseline-opt',
    'rewrite',
    #'baseline',
    #'baseline-no_opt',
]

## Data processing functions
# Normalizing function
def normalize(field1, field2, scale):
    return np.asarray(field1)/np.asarray(field2)

# Absolute difference with scaling function
def abs_diff(field1, field2, scale):
    return (np.asarray(field1) - np.asarray(field2)) / scale

# Proportional difference function with percentage or ratio
def prop_diff(field1, field2, percent= True):
    return (np.asarray(field1) - np.asarray(field2)) * (100 if percent else 1) / np.asarray(field2)

## Data gather functions
# Fill relevant fields with 0s to plot
def dummy_data(data_vector):
    data_vector.update({
        'hls_lut':0,
        'hls_ff':0,
        'hls_bram':0,
        'hls_dsp':0,
        'est_lut':0,
        'est_ff':0,
        'est_bram':0,
        'est_dsp':0,
        'full_lut':0,
        'full_ff':0,
        'full_bram':0,
        'full_dsp':0,
        'hls_lat_min':0,
        'hls_lat_max':0,
    })
    return data_vector

## Plotting functions
def plot_resources(data, results_json):
    # Create data vectors
    baseline_bench_list    = [r['bench'] for r in data if r['version'] == SELECT_BENCHES[0]]
    baseline_hls_lut       = list(map(int,   [r['hls_lut']   for r in data if r['version'] == SELECT_BENCHES[0]]))
    rewrite_hls_lut        = list(map(int,   [r['hls_lut']   for r in data if r['version'] == SELECT_BENCHES[1]]))
    baseline_hls_ff        = list(map(int,   [r['hls_ff']    for r in data if r['version'] == SELECT_BENCHES[0]]))
    rewrite_hls_ff         = list(map(int,   [r['hls_ff']    for r in data if r['version'] == SELECT_BENCHES[1]]))
    baseline_hls_bram      = list(map(int,   [r['hls_bram']  for r in data if r['version'] == SELECT_BENCHES[0]]))
    rewrite_hls_bram       = list(map(int,   [r['hls_bram']  for r in data if r['version'] == SELECT_BENCHES[1]]))
    baseline_hls_dsp       = list(map(int,   [r['hls_dsp']   for r in data if r['version'] == SELECT_BENCHES[0]]))
    rewrite_hls_dsp        = list(map(int,   [r['hls_dsp']   for r in data if r['version'] == SELECT_BENCHES[1]]))
    if data[0]['estimate']:
        if hasattr(data[0], 'est_lut'):
            baseline_est_lut   = list(map(int,   [r['est_lut']   for r in data if r['version'] == SELECT_BENCHES[0]]))
            rewrite_est_lut    = list(map(int,   [r['est_lut']   for r in data if r['version'] == SELECT_BENCHES[1]]))
            baseline_est_ff    = list(map(int,   [r['est_ff']    for r in data if r['version'] == SELECT_BENCHES[0]]))
            rewrite_est_ff     = list(map(int,   [r['est_ff']    for r in data if r['version'] == SELECT_BENCHES[1]]))
            baseline_est_bram  = list(map(int,   [r['est_bram']  for r in data if r['version'] == SELECT_BENCHES[0]]))
            rewrite_est_bram   = list(map(int,   [r['est_bram']  for r in data if r['version'] == SELECT_BENCHES[1]]))
            baseline_est_dsp   = list(map(int,   [r['est_dsp']   for r in data if r['version'] == SELECT_BENCHES[0]]))
            rewrite_est_dsp    = list(map(int,   [r['est_dsp']   for r in data if r['version'] == SELECT_BENCHES[1]]))
    else:
        baseline_full_lut  = list(map(int,   [r['full_lut']  for r in data if r['version'] == SELECT_BENCHES[0]]))
        rewrite_full_lut   = list(map(int,   [r['full_lut']  for r in data if r['version'] == SELECT_BENCHES[1]]))
        baseline_full_ff   = list(map(int,   [r['full_ff']   for r in data if r['version'] == SELECT_BENCHES[0]]))
        rewrite_full_ff    = list(map(int,   [r['full_ff']   for r in data if r['version'] == SELECT_BENCHES[1]]))
        baseline_full_bram = list(map(float, [r['full_bram'] for r in data if r['version'] == SELECT_BENCHES[0]]))
        rewrite_full_bram  = list(map(float, [r['full_bram'] for r in data if r['version'] == SELECT_BENCHES[1]]))
        baseline_full_dsp  = list(map(int,   [r['full_dsp']  for r in data if r['version'] == SELECT_BENCHES[0]]))
        rewrite_full_dsp   = list(map(int,   [r['full_dsp']  for r in data if r['version'] == SELECT_BENCHES[1]]))

    # Remove selected benches
    indices = []
    for index, r in enumerate(baseline_bench_list):
        if r in MARKED_BENCHES:
            indices.append(index)
    indices.sort()
    indices.reverse()
    for index in indices:
        del baseline_bench_list[index]
        del baseline_hls_lut[index]
        del rewrite_hls_lut[index]
        del baseline_hls_ff[index]
        del rewrite_hls_ff[index]
        del baseline_hls_bram[index]
        del rewrite_hls_bram[index]
        del baseline_hls_dsp[index]
        del rewrite_hls_dsp[index]
        if data[0]['estimate']:
            if hasattr(data[0], 'est_lut'):
                del baseline_est_lut[index]
                del rewrite_est_lut[index]
                del baseline_est_ff[index]
                del rewrite_est_ff[index]
                del baseline_est_bram[index]
                del rewrite_est_bram[index]
                del baseline_est_dsp[index]
                del rewrite_est_dsp[index]
        else:
            del baseline_full_lut[index]
            del rewrite_full_lut[index]
            del baseline_full_ff[index]
            del rewrite_full_ff[index]
            del baseline_full_bram[index]
            del rewrite_full_bram[index]
            del baseline_full_dsp[index]
            del rewrite_full_dsp[index]

    # Plot HLS LUTs and FFs
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 4,
        'mean'       : 0,
        'base'       : False,
        'color'      : 0,
        'ref_1'      : baseline_hls_lut,
        'ref_2'      : rewrite_hls_lut,
        'ref_3'      : baseline_hls_ff,
        'ref_4'      : rewrite_hls_ff,
        'bar_labels' : ['HLS C++ LUTs', 'Fuse LUTs', 'HLS C++ FFs', 'Fuse FFs'],
        'plot_labels': ['MachSuite HLS LUTs and FFs','Benchmarks','LUTs and FFs from HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_luts-ffs.pdf')
    }
    plot_n_fields(plot_data)

    # Plot HLS BRAMs and DSPs
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 4,
        'mean'       : 0,
        'base'       : False,
        'color'      : 0,
        'ref_1'      : baseline_hls_bram,
        'ref_2'      : rewrite_hls_bram,
        'ref_3'      : baseline_hls_dsp,
        'ref_4'      : rewrite_hls_dsp,
        'bar_labels' : ['HLS C++ BRAMs', 'Fuse BRAMs', 'HLS C++ DSPs', 'Fuse DSPs'],
        'plot_labels': ['MachSuite HLS BRAMs and DSPs','Benchmarks','BRAMs and DSPs from HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_brams-dsps.pdf')
    }
    plot_n_fields(plot_data)

    if data[0]['estimate']:
        if hasattr(data[0], 'est_lut'):
    # Plot EST LUTs and FFs
            plot_data = {
                'benches'    : baseline_bench_list,
                'fields'     : 4,
                'mean'       : 0,
                'base'       : False,
                'color'      : 0,
                'ref_1'      : baseline_est_lut,
                'ref_2'      : rewrite_est_lut,
                'ref_3'      : baseline_est_ff,
                'ref_4'      : rewrite_est_ff,
                'bar_labels' : ['HLS C++ LUTs', 'Fuse LUTs', 'HLS C++ FFs', 'Fuse FFs'],
                'plot_labels': ['MachSuite LUTs and FFs Estimates','Benchmarks','LUTs and FFs from HLS'],
                'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_est_luts-ffs.pdf')
            }
            plot_n_fields(plot_data)

    # Plot EST BRAMs and DSPs
            plot_data = {
                'benches'    : baseline_bench_list,
                'fields'     : 4,
                'mean'       : 0,
                'base'       : False,
                'color'      : 0,
                'ref_1'      : baseline_est_bram,
                'ref_2'      : rewrite_est_bram,
                'ref_3'      : baseline_est_dsp,
                'ref_4'      : rewrite_est_dsp,
                'bar_labels' : ['HLS C++ BRAMs', 'Fuse BRAMs', 'HLS C++ DSPs', 'Fuse DSPs'],
                'plot_labels': ['MachSuite BRAMs and DSPs Estimates','Benchmarks','BRAMs and DSPs from HLS'],
                'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_est_brams-dsps.pdf')
            }
            plot_n_fields(plot_data)
    else:
    # Plot FULL LUTs and FFs
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 4,
            'mean'       : 0,
            'base'       : False,
            'color'      : 0,
            'ref_1'      : baseline_full_lut,
            'ref_2'      : rewrite_full_lut,
            'ref_3'      : baseline_full_ff,
            'ref_4'      : rewrite_full_ff,
            'bar_labels' : ['HLS C++ LUTs', 'Fuse LUTs', 'HLS C++ FFs', 'Fuse FFs'],
            'plot_labels': ['MachSuite LUTs and FFs','Benchmarks','LUTs and FFs from synthesis'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_luts-ffs.pdf')
        }
        plot_n_fields(plot_data)

    # Plot FULL BRAMs and DSPs
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 4,
            'mean'       : 0,
            'base'       : False,
            'color'      : 0,
            'ref_1'      : baseline_full_bram,
            'ref_2'      : rewrite_full_bram,
            'ref_3'      : baseline_full_dsp,
            'ref_4'      : rewrite_full_dsp,
            'bar_labels' : ['HLS C++ BRAMs', 'Fuse DSPs', 'HLS C++ BRAMs', 'Fuse DSPs'],
            'plot_labels': ['MachSuite BRAMs and DSPs','Benchmarks','BRAMs and DSPs from synthesis'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_brams-dsps.pdf')
        }
        plot_n_fields(plot_data)

    # Normalized vectors
    norm_hls_lut   = np.asarray(rewrite_hls_lut)  /np.asarray(baseline_hls_lut)
    norm_hls_ff    = np.asarray(rewrite_hls_ff)   /np.asarray(baseline_hls_ff)
    norm_hls_bram  = np.asarray(rewrite_hls_bram) /np.asarray(baseline_hls_bram)
    norm_hls_dsp   = np.asarray(rewrite_hls_dsp)  /np.asarray(baseline_hls_dsp)
    if not data[0]['estimate']:
        norm_full_lut  = np.asarray(rewrite_full_lut) /np.asarray(baseline_full_lut)
        norm_full_ff   = np.asarray(rewrite_full_ff)  /np.asarray(baseline_full_ff)
        norm_full_bram = np.asarray(rewrite_full_bram)/np.asarray(baseline_full_bram)
        norm_full_dsp  = np.asarray(rewrite_full_dsp) /np.asarray(baseline_full_dsp)
    
    # Plot normalized rewrite hls resources
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 4,
        'mean'       : 2,
        'base'       : True,
        'basepoint'  : 1,
        'color'      : 0,
        'ref_1'      : norm_hls_lut,
        'ref_2'      : norm_hls_ff,
        'ref_3'      : norm_hls_bram,
        'ref_4'      : norm_hls_dsp,
        'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
        'plot_labels': ['Normalized Plots','Benchmarks','resource utilization from HLS normalized'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_normalized.pdf')
    }
    subplot_n_fields(plot_data)

    # Plot normalized rewrite synthesis resources
    if not data[0]['estimate']:
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 4,
            'mean'       : 2,
            'base'       : True,
            'basepoint'  : 1,
            'color'      : 0,
            'ref_1'      : norm_full_lut,
            'ref_2'      : norm_full_ff,
            'ref_3'      : norm_full_bram,
            'ref_4'      : norm_full_dsp,
            'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
            'plot_labels': ['Normalized Plots','Benchmarks','resource utilization from synthesis normalized'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_normalized.pdf')
        }
        subplot_n_fields(plot_data)
        
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 1,
            'mean'       : 2,
            'base'       : True,
            'basepoint'  : 1,
            'color'      : None,
            'ref_1'      : norm_full_lut,
            'bar_labels' : ['LUTs'],
            'plot_labels': ['Normalized Plots','Benchmarks','Normalized LUT count'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_normalized_luts.pdf')
        }
        plot_n_fields(plot_data)
    
    # Absolute Differential vectors
    absdiff_hls_lut   = abs_diff(rewrite_hls_lut , baseline_hls_lut , 1)
    absdiff_hls_ff    = abs_diff(rewrite_hls_ff  , baseline_hls_ff  , 1)
    absdiff_hls_bram  = abs_diff(rewrite_hls_bram, baseline_hls_bram, 1)
    absdiff_hls_dsp   = abs_diff(rewrite_hls_dsp , baseline_hls_dsp , 1)
    if not data[0]['estimate']:
        absdiff_full_lut  = abs_diff(rewrite_full_lut , baseline_full_lut , 1)
        absdiff_full_ff   = abs_diff(rewrite_full_ff  , baseline_full_ff  , 1)
        absdiff_full_bram = abs_diff(rewrite_full_bram, baseline_full_bram, 1)
        absdiff_full_dsp  = abs_diff(rewrite_full_dsp , baseline_full_dsp , 1)
    
    # Edit vectors for missing and error benches
    bench_list  = [r['bench']  for r in data ]
    status_list = [r['status'] for r in data ]
    for index, r in enumerate(bench_list):
        if status_list[index] is not 'ok':
            absdiff_hls_lut[index//2] = 0   
            absdiff_hls_ff[index//2] = 0   
            absdiff_hls_bram[index//2] = 0   
            absdiff_hls_dsp[index//2] = 0   
            if not data[0]['estimate']:
                absdiff_full_lut[index//2] = 0   
                absdiff_full_ff[index//2] = 0   
                absdiff_full_bram[index//2] = 0   
                absdiff_full_dsp[index//2] = 0   

    # Plot absolute differential rewrite hls resources
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 4,
        'mean'       : 0,
        'base'       : True,
        'basepoint'  : 0,
        'color'      : 0,
        'ref_1'      : absdiff_hls_lut,
        'ref_2'      : absdiff_hls_ff,
        'ref_3'      : absdiff_hls_bram,
        'ref_4'      : absdiff_hls_dsp,
        'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
        'plot_labels': ['Absolute Difference in Plots','Benchmarks','increase in resource utilization from HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_absdiff.pdf')
    }
    subplot_n_fields(plot_data)

    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 1,
        'mean'       : 0,
        'base'       : True,
        'basepoint'  : 0,
        'color'      : 0,
        'ref_1'      : absdiff_hls_lut,
        'bar_labels' : ['LUTs'],
        'plot_labels': ['Absolute Difference Plot','Benchmarks','increase in LUTs from baseline in HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_absdiff_luts.pdf')
    }
    plot_n_fields(plot_data)
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 1,
        'mean'       : 0,
        'base'       : True,
        'basepoint'  : 0,
        'color'      : 1,
        'ref_1'      : absdiff_hls_ff,
        'bar_labels' : ['FFs'],
        'plot_labels': ['Absolute Difference Plots','Benchmarks','increase in registers from baseline in HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_absdiff_ffs.pdf')
    }
    plot_n_fields(plot_data)
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 1,
        'mean'       : 0,
        'base'       : True,
        'basepoint'  : 0,
        'color'      : 2,
        'ref_1'      : absdiff_hls_bram,
        'bar_labels' : ['BRAMs'],
        'plot_labels': ['Absolute Difference Plots','Benchmarks','increase in BRAMs from baseline in HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_absdiff_brams.pdf')
    }
    plot_n_fields(plot_data)
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 1,
        'mean'       : 0,
        'base'       : True,
        'basepoint'  : 0,
        'color'      : 3,
        'ref_1'      : absdiff_hls_dsp,
        'bar_labels' : ['DSPs'],
        'plot_labels': ['Absolute Difference Plots','Benchmarks','increase in DSPs from baseline in HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_absdiff_dsps.pdf')
    }
    plot_n_fields(plot_data)
    # Plot absolute differential rewrite synthesis resources
    if not data[0]['estimate']:
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 4,
            'mean'       : 0,
            'base'       : True,
            'basepoint'  : 0,
            'color'      : 0,
            'ref_1'      : absdiff_full_lut,
            'ref_2'      : absdiff_full_ff,
            'ref_3'      : absdiff_full_bram,
            'ref_4'      : absdiff_full_dsp,
            'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
            'plot_labels': ['Absolute Difference in Plots','Benchmarks','increase in resource utilization from synthesis'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_absdiff.pdf')
        }
        subplot_n_fields(plot_data)

        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 1,
            'mean'       : 0,
            'base'       : True,
            'basepoint'  : 0,
            'color'      : None,
            'ref_1'      : absdiff_full_lut,
            'bar_labels' : ['LUTs'],
            'plot_labels': ['Absolute Difference Plots','Benchmarks','Change in LUTs'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_absdiff_luts.pdf')
        }
        plot_n_fields(plot_data)
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 1,
            'mean'       : 0,
            'base'       : True,
            'basepoint'  : 0,
            'color'      : None,
            'ref_1'      : absdiff_full_ff,
            'bar_labels' : ['FFs'],
            'plot_labels': ['Absolute Difference Plots','Benchmarks','Change in flip-flops'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_absdiff_ffs.pdf')
        }
        plot_n_fields(plot_data)
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 1,
            'mean'       : 0,
            'base'       : True,
            'basepoint'  : 0,
            'color'      : None,
            'ref_1'      : absdiff_full_bram,
            'bar_labels' : ['BRAMs'],
            'plot_labels': ['Absolute Difference Plots','Benchmarks','Change in BRAMs'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_absdiff_brams.pdf')
        }
        plot_n_fields(plot_data)
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 1,
            'mean'       : 0,
            'base'       : True,
            'basepoint'  : 0,
            'color'      : None,
            'ref_1'      : absdiff_full_dsp,
            'bar_labels' : ['DSPs'],
            'plot_labels': ['Absolute Difference Plots','Benchmarks','Change in DSPs'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_absdiff_dsps.pdf')
        }
        plot_n_fields(plot_data)

    # Proportional Differential vectors
    propdiff_hls_lut   = prop_diff(rewrite_hls_lut , baseline_hls_lut)
    propdiff_hls_ff    = prop_diff(rewrite_hls_ff  , baseline_hls_ff)
    propdiff_hls_bram  = prop_diff(rewrite_hls_bram, baseline_hls_bram)
    propdiff_hls_dsp   = prop_diff(rewrite_hls_dsp , baseline_hls_dsp)
    if not data[0]['estimate']:
        propdiff_full_lut  = prop_diff(rewrite_full_lut , baseline_full_lut)
        propdiff_full_ff   = prop_diff(rewrite_full_ff  , baseline_full_ff)
        propdiff_full_bram = prop_diff(rewrite_full_bram, baseline_full_bram)
        propdiff_full_dsp  = prop_diff(rewrite_full_dsp , baseline_full_dsp)
    
    # Edit vectors for missing and error benches
    bench_list  = [r['bench']  for r in data ]
    status_list = [r['status'] for r in data ]
    for index, r in enumerate(bench_list):
        if status_list[index] is not 'ok':
            propdiff_hls_lut[index//2] = 0   
            propdiff_hls_ff[index//2] = 0   
            propdiff_hls_bram[index//2] = 0   
            propdiff_hls_dsp[index//2] = 0   
            if not data[0]['estimate']:
                propdiff_full_lut[index//2] = 0   
                propdiff_full_ff[index//2] = 0   
                propdiff_full_bram[index//2] = 0   
                propdiff_full_dsp[index//2] = 0   

    # Plot proportional differential rewrite hls resources
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 4,
        'mean'       : 2,
        'base'       : False,
        'color'      : 0,
        'ref_1'      : propdiff_hls_lut,
        'ref_2'      : propdiff_hls_ff,
        'ref_3'      : propdiff_hls_bram,
        'ref_4'      : propdiff_hls_dsp,
        'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
        'plot_labels': ['Proportional Difference in Plots','Benchmarks','percentage increase in resource utilization from HLS'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_propdiff.pdf')
    }
    plot_n_fields(plot_data)

    # Plot proportional differential rewrite synthesis resources
    if not data[0]['estimate']:
        plot_data = {
            'benches'    : baseline_bench_list,
            'fields'     : 4,
            'mean'       : 2,
            'base'       : False,
            'color'      : 0,
            'ref_1'      : propdiff_full_lut,
            'ref_2'      : propdiff_full_ff,
            'ref_3'      : propdiff_full_bram,
            'ref_4'      : propdiff_full_dsp,
            'bar_labels' : ['LUTs', 'FFs', 'BRAMs', 'DSPs'],
            'plot_labels': ['Proportional Difference in Plots','Benchmarks','percentage increase in resource utilization from synthesis'],
            'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_propdiff.pdf')
        }
        plot_n_fields(plot_data)

def plot_latencies(data, results_json):
    # Create data vectors
    baseline_bench_list    = [r['bench'] for r in data if r['version'] == SELECT_BENCHES[0]]
    baseline_hls_maxlat    = list(map(int,   [r['hls_lat_max'] if r['hls_lat_max'] is not None else 0 for r in data if r['version'] == SELECT_BENCHES[0]]))
    rewrite_hls_maxlat     = list(map(int,   [r['hls_lat_max'] if r['hls_lat_max'] is not None else 0 for r in data if r['version'] == SELECT_BENCHES[1]]))

    # Remove selected benches
    indices = []
    for index, r in enumerate(baseline_bench_list):
        if r in MARKED_BENCHES:
            indices.append(index)
    indices.sort()
    indices.reverse()
    for index in indices:
        del baseline_bench_list[index]
        del baseline_hls_maxlat[index]
        del rewrite_hls_maxlat[index]

    # Plot Max Latency
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 2,
        'mean'       : 0,
        'base'       : False,
        'color'      : 0,
        'ref_1'      : baseline_hls_maxlat,
        'ref_2'      : rewrite_hls_maxlat,
        'bar_labels' : ['HLS C++ Max Latency', 'Fuse Max Latency'],
        'plot_labels': ['MachSuite Latencies','Benchmarks','Estimated Latency in clock cycles'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_latencies.pdf')
    }
    plot_n_fields(plot_data)

    # Normalized vectors
    norm_hls_maxlat  = np.asarray(rewrite_hls_maxlat)  /np.asarray(baseline_hls_maxlat)
    
    # Plot normalized rewrite hls resources
    plot_data = {
        'benches'    : baseline_bench_list,
        'fields'     : 1,
        'mean'       : 2,
        'base'       : True,
        'basepoint'  : 1,
        'color'      : None,
        'ref_1'      : norm_hls_maxlat,
        'bar_labels' : ['Latency'],
        'plot_labels': ['Normalized Plots','Benchmarks','Normalized latency'],
        'figure_name': os.path.join(os.path.dirname(results_json),'machsuite_hls_latencies_normalized.pdf')
    }
    plot_n_fields(plot_data)

def plot(results_json):
    # Load JSON data.
    with open(results_json) as f:
        results_data = json.load(f)

    # Generate summaries.
    data = [summarize_one(j) for j in results_data.values()]

    # Remove "blank" lines for missing benchmark versions.
    for missing in find_missing(data, SELECT_BENCHES[0], SELECT_BENCHES[1]):
        print('{} is missing'.format(missing))
        missing_data = {
            'bench':missing,
    	    'version':SELECT_BENCHES[1],
            'status':'missing',
        }
        data.append(dummy_data(missing_data))

    for error in {r['bench'] for r in data if r['status'] == 'error'}:
        print('{} has failed'.format(error))
    data = [r if r['status'] is not 'error' else dummy_data(r) for r in data]

    # Sort to group benchmarks together.
    data.sort(key=lambda r: (r['bench'], r['version']))
    
    # Plot resources.
    plot_resources(data, results_json)
    plot_latencies(data, results_json)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Give the path to the results.json file create plots.\n'
              'Usage: {} _results/<batch>/results.json'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)

    plot(sys.argv[1])
