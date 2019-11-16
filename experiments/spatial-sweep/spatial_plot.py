import re
import csv
import json

from rpt import RPTParser
import pandas as pd
import csv
import numpy as np
import matplotlib.pyplot as plt

#PERF_STATS = ['used', 'total']

def _find_row(table, key, value):
    """Given a list of dicts in `table`, find the dict whose value for
    `key` contains `value`.
    """
    for row in table:
        if value in row[key]:
            return row
    raise ValueError("row not found")

def spatial_report(filepath):
    parser = RPTParser(filepath)

    logic_table = parser.get_table(
        re.compile(r'1\. Slice Logic'), 2, skip=1)
    lut = _find_row(logic_table, 'Site Type', 'LUTs')
    mem_table = parser.get_table(
        re.compile(r'\. Memory'), 2, skip=1)
    dsp_table = parser.get_table(
        re.compile(r'\. DSP'), 2, skip=1)
    used_key = 'Used'
    avail_key = 'Available'

    lut = _find_row(logic_table, 'Site Type', 'LUTs')
    lut_logic = _find_row(logic_table, 'Site Type', 'Logic')
    lut_mem = _find_row(logic_table, 'Site Type', 'Memory')
    reg = _find_row(logic_table, 'Site Type', 'Registers')
    reg_ff = _find_row(logic_table, 'Site Type', 'Flip Flop')
    reg_latch = _find_row(logic_table, 'Site Type', 'Latch')

    bram_tile = _find_row(mem_table, 'Site Type', 'Block RAM Tile')
    bram36 = _find_row(mem_table, 'Site Type', 'RAMB36/')
    bram18 = _find_row(mem_table, 'Site Type', 'RAMB18')

    dsp = _find_row(dsp_table, 'Site Type', 'DSPs')

    return {
        'lut_used':        int(lut[used_key]),
        'lut_avail':       int(lut[avail_key]),
        'lut_logic_used':  int(lut_logic[used_key]),
        'lut_logic_avail': int(lut_logic[avail_key]),
        'lut_mem_used':    int(lut_mem[used_key]),
        'lut_mem_avail':   int(lut_mem[avail_key]),
        'reg_used':        int(reg[used_key]),
        'reg_avail':       int(reg[avail_key]),
        'reg_ff_used':     int(reg_ff[used_key]),
        'reg_ff_avail':    int(reg_ff[avail_key]),
        'reg_latch_used':  int(reg_latch[used_key]),
        'reg_latch_avail': int(reg_latch[avail_key]),
        'bram_tile_used':  float(bram_tile[used_key]),
        'bram_tile_avail': int(bram_tile[avail_key]),
        'bram36_used':     int(bram36[used_key]),
        'bram36_avail':    int(bram36[avail_key]),
        'bram18_used':     int(bram18[used_key]),
        'bram18_avail':    int(bram18[avail_key]),
        'dsp_used':        int(dsp[used_key]),
        'dsp_avail':       int(dsp[avail_key]),
    }
import seaborn as sns
cl = sns.color_palette("colorblind",6)
PERF_RESOURCES = ['dsp_used', 'bram_tile_used','lut_used', 'reg_used'] 
#PERF_RESOURCES = ['lut_used', 'reg_used']
PERF_AVAIL = ['dsp_avail', 'bram_tile_avail', 'lut_avail', 'reg_avail']
plot_res = {r:[] for r in PERF_RESOURCES}

for i in range(1,17):
    resources = spatial_report("spatial-quickstart/gen/GEMM_NCubed_"+str(i)+"/verilog-zynq/par_utilization.rpt")
    for r, a in zip(PERF_RESOURCES, PERF_AVAIL):
        if i == 1: plot_res[r].append(resources[r])
        plot_res[r].append(resources[r]/plot_res[r][0])#/resources[a])

fig,ax1 = plt.subplots()
loop_k = np.arange(16)+1
for i, r in enumerate(plot_res.keys()):
    ax1.plot(loop_k, plot_res[r][1:], color = cl[i])
ax1.set_ylabel('Normalized Resource Usages')
ax1.set_xlabel('Unrolling Factor')
PERF_RESOURCES_NAME = ['dsp used', 'bram_tile used','lut used', 'reg used'] 
ax1.legend(PERF_RESOURCES_NAME)
marker = {'input_matrix_a':'s','input_matrix_b':'+'}
leg2 = ['input matrix a','input matrix b']
ax2 = ax1.twinx()
for i, sram in enumerate(marker):
    df = pd.read_csv(sram+'.csv')
    bank = [ eval(re.sub("\s+", ",", n.strip()))[0] for n in df[' N']]
    ax2.plot(loop_k,bank, marker[sram], color=cl[i+4])
ax2.legend(leg2,loc=4)
ax2.set_ylabel('Banking Decisions')
plt.savefig('paper-normalized.pdf', bbox_inches='tight')
plt.show()
