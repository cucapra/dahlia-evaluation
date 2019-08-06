import re
import xml.etree.ElementTree as ET
import pandas as pd

from rpt import RPTParser


PERF_RESOURCES = ['dsp', 'bram', 'lut', 'ff']
PERF_STATS = ['used', 'total']


def performance_estimates(filepath):
    """
    perf.est files are xml files. We parse the file as XML data and extract
    the hwLatency field and resource fields
    """

    with open(filepath) as file:
        src = file.read()
    root = ET.fromstring(src)

    out = {}

    # Extract hardware latency.
    hw = ''
    graph = root.find('graph')
    if graph:
        hw = graph.attrib['hwLatency']
    out['hw_latency'] = hw

    # Extract all the resources.
    for resource in PERF_RESOURCES:
        el = root.find("./resources/resource[@name='{}']".format(resource))
        for stat in PERF_STATS:
            value = el.attrib[stat] if el is not None else ''
            out["{}_{}".format(resource, stat)] = value

    return out


def hls_report(filepath):
    parser = RPTParser(filepath)

    timing_table = parser.get_table(
        re.compile(r'\+ Timing \(ns\)'), 1)
    latency_table = parser.get_table(
        re.compile(r'\+ Latency \(clock cycles\)'), 1, True)
    utilization_table = parser.get_table(
        re.compile(r'== Utilization Estimates'), 2)

    # Extract relevant data.
    return {
        'target_clock':    timing_table.iloc[0,1],
        'estimated_clock': timing_table.iloc[0,2],
        'min_latency':     latency_table.iloc[0,0],
        'max_latency':     latency_table.iloc[0,1],
        'pipelining':      latency_table.iloc[0,4],
        'bram_used':       utilization_table.iloc[utilization_table.loc[utilization_table.iloc[:,0] == 'Total'].index[0],1],
        'dsp48_used':      utilization_table.iloc[utilization_table.loc[utilization_table.iloc[:,0] == 'Total'].index[0],2],
        'ff_used':         utilization_table.iloc[utilization_table.loc[utilization_table.iloc[:,0] == 'Total'].index[0],3],
        'lut_used':        utilization_table.iloc[utilization_table.loc[utilization_table.iloc[:,0] == 'Total'].index[0],4]
    }


def sds_report(filepath):
    parser = RPTParser(filepath)

    logic_table = parser.get_table(
        re.compile(r'\. Slice Logic'), 2)
    mem_table = parser.get_table(
        re.compile(r'\. Memory'), 2)
    dsp_table = parser.get_table(
        re.compile(r'\. DSP'), 2)

    used_idx = 'Used'
    avail_idx = 'Available'
    #print(logic_table.loc[logic_table.iloc[:,0].str.contains('LUTs'  ), used_idx])

    return {
        'lut_used':        logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('LUTs'  )   ].index[0],used_idx],
        'lut_avail':       logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('LUTs'  )   ].index[0],avail_idx],
        'lut_logic_used':  logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Logic' )   ].index[0],used_idx],
        'lut_logic_avail': logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Logic' )   ].index[0],avail_idx],
        'lut_mem_used':    logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Memory')   ].index[0],used_idx],
        'lut_mem_avail':   logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Memory')   ].index[0],avail_idx],
        'reg_used':        logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Registers')].index[0],used_idx],
        'reg_avail':       logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Registers')].index[0],avail_idx],
        'reg_ff_used':     logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Flip Flop')].index[0],used_idx],
        'reg_ff_avail':    logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Flip Flop')].index[0],avail_idx],
        'reg_latch_used':  logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Latch')    ].index[0],used_idx],
        'reg_latch_avail': logic_table.loc[logic_table.loc[logic_table.iloc[:,0].str.contains('Latch')    ].index[0],avail_idx],
        'bram_tile_used':  mem_table.loc[mem_table.loc[mem_table.iloc[:,0].str.contains('Block RAM')      ].index[0],used_idx],
        'bram_tile_avail': mem_table.loc[mem_table.loc[mem_table.iloc[:,0].str.contains('Block RAM')      ].index[0],avail_idx],
        'bram36_used':     mem_table.loc[mem_table.loc[mem_table.iloc[:,0].str.contains('RAMB36/')        ].index[0],used_idx],
        'bram36_avail':    mem_table.loc[mem_table.loc[mem_table.iloc[:,0].str.contains('RAMB36/')        ].index[0],avail_idx],
        'bram18_used':     mem_table.loc[mem_table.loc[mem_table.iloc[:,0] == 'RAMB18'                    ].index[0],used_idx],
        'bram18_avail':    mem_table.loc[mem_table.loc[mem_table.iloc[:,0] == 'RAMB18'                    ].index[0],avail_idx],
        'dsp_used':        dsp_table.loc[dsp_table.loc[dsp_table.iloc[:,0].str.contains('DSPs')           ].index[0],used_idx],
        'dsp_avail':       dsp_table.loc[dsp_table.loc[dsp_table.iloc[:,0].str.contains('DSPs')           ].index[0],avail_idx],
    }


LOG_PATTERNS = {
    'missing file': 'No rule to make target',
    'compiler error': "Error when calling 'arm-linux-gnueabihf-g++",
    'timing': 'Design failed to meet timing',
    'resources': 'Place Check : This design requires more',
}


def log_messages(filepath):
    out = {k: False for k in LOG_PATTERNS}
    with open(filepath) as f:
        for line in f:
            for key, pat in LOG_PATTERNS.items():
                if pat in line:
                    out[key] = True
    return out
