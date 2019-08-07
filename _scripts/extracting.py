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

    # Get the (only) row in the table whose "Name" is "Total".
    util_total = utilization_table[utilization_table['Name'] == 'Total'] \
        .iloc[0]

    # Extract relevant data.
    return {
        'target_clock':    timing_table.loc[0, 'Target'],
        'estimated_clock': timing_table.loc[0, 'Estimated'],
        'min_latency':     latency_table.loc[0, 'Latency min'],
        'max_latency':     latency_table.loc[0, 'Latency max'],
        'pipelining':      latency_table.loc[0, 'Pipeline Type'],
        'bram_used':       util_total['BRAM_18K'],
        'dsp48_used':      util_total['DSP48E'],
        'ff_used':         util_total['FF'],
        'lut_used':        util_total['LUT'],
    }


def sds_report(filepath):
    parser = RPTParser(filepath)

    logic_table = parser.get_table(
        re.compile(r'\. Slice Logic'), 2)
    mem_table = parser.get_table(
        re.compile(r'\. Memory'), 2)
    dsp_table = parser.get_table(
        re.compile(r'\. DSP'), 2)

    used_key = 'Used'
    avail_key = 'Available'

    lut = logic_table[logic_table['Site Type'].str.contains('LUTs')].iloc[0]
    lut_logic = logic_table[logic_table['Site Type'].str.contains('Logic')].iloc[0]
    lut_mem = logic_table[logic_table['Site Type'].str.contains('Memory')].iloc[0]
    reg = logic_table[logic_table['Site Type'].str.contains('Registers')].iloc[0]
    reg_ff = logic_table[logic_table['Site Type'].str.contains('Flip Flop')].iloc[0]
    reg_latch = logic_table[logic_table['Site Type'].str.contains('Latch')].iloc[0]

    bram_tile = mem_table[mem_table['Site Type'].str.contains('Block RAM Tile')].iloc[0]
    bram36 = mem_table[mem_table['Site Type'].str.contains('RAMB36/')].iloc[0]
    bram18 = mem_table[mem_table['Site Type'].str.contains('RAMB18')].iloc[0]

    dsp = dsp_table[dsp_table['Site Type'].str.contains('DSPs')].iloc[0]

    return {
        'lut_used':        lut[used_key],
        'lut_avail':       lut[avail_key],
        'lut_logic_used':  lut_logic[used_key],
        'lut_logic_avail': lut_logic[avail_key],
        'lut_mem_used':    lut_mem[used_key],
        'lut_mem_avail':   lut_mem[avail_key],
        'reg_used':        reg[used_key],
        'reg_avail':       reg[avail_key],
        'reg_ff_used':     reg_ff[used_key],
        'reg_ff_avail':    reg_ff[avail_key],
        'reg_latch_used':  reg_latch[used_key],
        'reg_latch_avail': reg_latch[avail_key],
        'bram_tile_used':  bram_tile[used_key],
        'bram_tile_avail': bram_tile[avail_key],
        'bram36_used':     bram36[used_key],
        'bram36_avail':    bram36[avail_key],
        'bram18_used':     bram18[used_key],
        'bram18_avail':    bram18[avail_key],
        'dsp_used':        dsp[used_key],
        'dsp_avail':       dsp[avail_key],
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
