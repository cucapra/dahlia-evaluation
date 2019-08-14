#!/usr/bin/env python3

#import json
#import sys
#import os
import numpy as np
import matplotlib.pyplot as plt

# Colorblind safe, print friendly color scheme 3-class PuOr from http://colorbrewer2.org/
COLOR = ['#e66101', '#fdb863', '#b2abd2', '#5e3c99']

# Geometric mean function
def geo_mean(field):
    a = np.array(field)
    return a.prod()**(1.0/len(a))

# Mean function
def mean(field):
    a = np.array(field)
    return field.sum()/len(field)

# Provide 2 or 4 vectors to plot a bar graph
def plot_n_fields(data):
    # Create figure
    fig = plt.figure()
    #fig, ax = plt.subplots()
    #ax1 = plt.subplot2grid((3, 3), (0, 0))

    # Add Mean or Geo-mean
    bench_list = data['benches'].copy()
    if data['mean'] == 1:
        bench_list.append('mean')
        data['ref_1'] = np.append(data['ref_1'], mean(data['ref_1']))
        if data['fields'] >= 2:
            data['ref_2'] = np.append(data['ref_2'], mean(data['ref_2']))
        if data['fields'] == 4:
            data['ref_3'] = np.append(data['ref_3'], mean(data['ref_3']))
            data['ref_4'] = np.append(data['ref_4'], mean(data['ref_4']))
    elif data['mean'] == 2:
        bench_list.append('geomean')
        data['ref_1'] = np.append(data['ref_1'], geo_mean(data['ref_1']))
        if data['fields'] >= 2:
            data['ref_2'] = np.append(data['ref_2'], geo_mean(data['ref_2']))
        if data['fields'] == 4:
            data['ref_3'] = np.append(data['ref_3'], geo_mean(data['ref_3']))
            data['ref_4'] = np.append(data['ref_4'], geo_mean(data['ref_4']))
    
    # Create stuff needed for plotting
    index = np.arange(len(bench_list))
    bar_width = 0.5 if data['fields'] == 1 else 0.4 if data['fields'] == 2 else 0.2 if data['fields'] == 4 else print('Plotting only supports 2 and 4 vectors')
    opacity = 0.8
    font_size = 18
    params = {#'backend'        : 'ps',
              'axes.labelsize' : font_size,
              'font.size'      : font_size, 
              'legend.fontsize': font_size,
              'xtick.labelsize': font_size,
              'ytick.labelsize': font_size} 
    plt.rcParams.update(params)
    bar_offset = [0] if data['fields'] == 1 else [-.5, +.5] if data['fields'] == 2 else [-1.5, -.5, +.5, +1.5] if data['fields'] == 4 else print('Plotting only supports 2 and 4 vectors')

    # Plot reference field 1
    bar1     = plt.bar(index + bar_offset[0] * bar_width, data['ref_1'] , bar_width, align='center', alpha=opacity, color=COLOR[data['color']], label=data['bar_labels'][0])
    
    # Plot comparing field 1
    if data['fields'] >= 2:
        bar2     = plt.bar(index + bar_offset[1] * bar_width, data['ref_2'], bar_width, align='center', alpha=opacity, color=COLOR[1], label=data['bar_labels'][1])
    
    if data['fields'] == 4:
        # Plot reference field 2
        bar3 = plt.bar(index + bar_offset[2] * bar_width, data['ref_3'] , bar_width, align='center', alpha=opacity, color=COLOR[2], label=data['bar_labels'][2])
        
        # Plot comparing field 2
        bar4 = plt.bar(index + bar_offset[3] * bar_width, data['ref_4'], bar_width, align='center', alpha=opacity, color=COLOR[3], label=data['bar_labels'][3])
    
    # Plot settings
    #plt.xlabel(data['plot_labels'][1])
    plt.xticks(index, bench_list)
    plt.ylabel(data['plot_labels'][2])
    plt.legend()
    #plt.title(data['plot_labels'][0])
    
    # Create plot and close it after an interval
    plt.gca().yaxis.grid(True)
    if data['base']:
        plt.gca().axhline(y=data['basepoint'], color='k') 
    plt.tight_layout()
    fig.autofmt_xdate()
    #plt.show(block=False)
    #plt.pause(5)
    #plt.close()

    # Create a figure and save it
    fig.set_figheight(6)
    fig.set_figwidth(10)
    fig.subplots_adjust(left=0.0)
    fig.subplots_adjust(bottom=0.0)
    #grid(True, color='#999999')
    fig.savefig(data['figure_name'], bbox_inches='tight', pad_inches=0.1)

# Provide 2 or 4 vectors to plot a bar graph
def subplot_n_fields(data):
    # Create figure
    fig, axs = plt.subplots(2, 2)

    # Add Mean or Geo-mean
    bench_list = data['benches'].copy()
    if data['mean'] == 1:
        bench_list.append('mean')
        data['ref_1'] = np.append(data['ref_1'], mean(data['ref_1']))
        data['ref_2'] = np.append(data['ref_2'], mean(data['ref_2']))
        if data['fields'] == 4:
            data['ref_3'] = np.append(data['ref_3'], mean(data['ref_3']))
            data['ref_4'] = np.append(data['ref_4'], mean(data['ref_4']))
    elif data['mean'] == 2:
        bench_list.append('geomean')
        data['ref_1'] = np.append(data['ref_1'], geo_mean(data['ref_1']))
        data['ref_2'] = np.append(data['ref_2'], geo_mean(data['ref_2']))
        if data['fields'] == 4:
            data['ref_3'] = np.append(data['ref_3'], geo_mean(data['ref_3']))
            data['ref_4'] = np.append(data['ref_4'], geo_mean(data['ref_4']))
    
    # Create stuff needed for plotting
    index = np.arange(len(bench_list))
    bar_width = 0.4 if data['fields'] == 2 else 0.2 if data['fields'] == 4 else print('Plotting only supports 2 and 4 vectors')
    opacity = 0.8
    font_size = 15
    params = {#'backend'        : 'ps',
              'axes.labelsize' : font_size,
              'font.size'      : font_size, 
              'legend.fontsize': font_size,
              'xtick.labelsize': 10,
              'ytick.labelsize': font_size} 
    plt.rcParams.update(params)

    # Plot reference field 1
    axs[0, 0].bar(index, data['ref_1'] , bar_width, align='center', alpha=opacity, color=COLOR[data['color']], label=data['bar_labels'][0])
    
    # Plot comparing field 1
    axs[0, 1].bar(index, data['ref_2'], bar_width, align='center', alpha=opacity, color=COLOR[1], label=data['bar_labels'][1])
    
    if data['fields'] == 4:
        # Plot reference field 2
        axs[1, 0].bar(index, data['ref_3'] , bar_width, align='center', alpha=opacity, color=COLOR[2], label=data['bar_labels'][2])
        
        # Plot comparing field 2
        axs[1, 1].bar(index, data['ref_4'], bar_width, align='center', alpha=opacity, color=COLOR[3], label=data['bar_labels'][3])
    
    # Plot settings
    for ax in axs.flat:
        #ax.set(xlabel=data['plot_labels'][1])
        #ax.set(ylabel=data['plot_labels'][2])
        ax.set_xticks(index)
        ax.set_xticklabels(bench_list)
        ax.legend()
        #ax.title(data['plot_labels'][0])
        ax.yaxis.grid(True)
        if data['base']:
            ax.axhline(y=data['basepoint'], color='k') 
    
    #fig.text(0.5, 0.04, data['plot_labels'][1], ha='center')
    fig.text(0.04, 0.5, data['plot_labels'][2], va='center', rotation='vertical')
    fig.autofmt_xdate()
    
    # Hide x labels and tick labels for top plots and y ticks for right plots.
    #for ax in axs.flat:
        #ax.label_outer()
    
    # Create a figure and save it
    fig.set_figheight(6)
    fig.set_figwidth(12)
    fig.subplots_adjust(left=0.125)
    fig.subplots_adjust(bottom=0.0)
    fig.savefig(data['figure_name'], bbox_inches='tight', pad_inches=0.1)

    plt.close(fig)
