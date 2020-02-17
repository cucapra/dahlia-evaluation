import re
import csv
import json


import pandas as pd
import csv
import numpy as np
import matplotlib.pyplot as plt

import seaborn as sns

cl = sns.color_palette("colorblind",6)
PERF_RESOURCES = ['dsp_used', 'bram_tile_used','lut_used'] 
PERF_AVAIL = ['dsp_avail', 'bram_tile_avail', 'lut_avail']

df = pd.read_csv('rpts/summary.csv')

plot_res = {r:[] for r in PERF_RESOURCES}

for i in range(1,17):
    for r, a in zip(PERF_RESOURCES, PERF_AVAIL):
        if i == 1: plot_res[r].append(df.loc[i-1][r])
        plot_res[r].append(df.loc[i-1][r]/plot_res[r][0])#/resources[a])

fig,ax1 = plt.subplots()
loop_k = np.arange(16)+1
maker = ['v','D','o']
for i, r in enumerate(plot_res.keys()):
    y = plot_res[r][1:]
    ax1.plot(loop_k, y, maker[i], ls='-', ms=8, color = cl[i],linewidth=3 )
size = 6
plt.xticks(fontsize = 16)
plt.yticks(fontsize = 16)
ax1.set_ylabel('Normalized Resource Usages',fontsize= 18)
ax1.set_xlabel('Unrolling Factor',fontsize = 18)
PERF_RESOURCES_NAME = ['DSP used', 'BRAM used','LUT used'] 
ax1.legend(PERF_RESOURCES_NAME,prop={'size': 16})
plt.savefig('paper-normalized.pdf', bbox_inches='tight')
plt.show()

fig,ax1 = plt.subplots()
marker = {'input_matrix_a':'s','input_matrix_b':'+'}
leg1 = ['input matrix a','input matrix b']
#ax2 = ax1.twinx()
ms = [8,11]
for i, sram in enumerate(marker):
    df = pd.read_csv('banks/'+sram+'.csv')
    bank = [ eval(re.sub("\s+", ",", n.strip()))[0] for n in df[' N']]
    ax1.plot(loop_k,bank, marker[sram], color=cl[i], ms = ms[i], mew=3)
ax1.legend(leg1,loc=4, prop={'size':16})
ax1.set_ylabel('Banking Decisions', fontsize = 18)
ax1.set_xlabel('Unrolling Factor',fontsize = 18)
plt.xticks(fontsize = 16)
plt.yticks(fontsize = 16)
#PERF_RESOURCES_NAME = ['DSP used', 'BRAM used','LUT used'] 
#ax1.legend(PERF_RESOURCES_NAME,prop={'size': 16})
plt.savefig('paper-banking.pdf', bbox_inches='tight')
plt.show()


