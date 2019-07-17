import re
import xml.etree.ElementTree as ET

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
        'target_clock':    timing_table[-1][1],
        'estimated_clock': timing_table[-1][2],
        'min_latency':     latency_table[-1][0],
        'max_latency':     latency_table[-1][1],
        'pipelining':      latency_table[-1][4],
        'bram_used':       utilization_table[-3][1],
        'dsp48_used':      utilization_table[-3][2],
        'ff_used':         utilization_table[-3][3],
        'lut_used':        utilization_table[-3][4]
    }


def sds_report(filepath):
    parser = RPTParser(filepath)

    logic_table = parser.get_table(
        re.compile(r'1. Slice Logic'), 2)
    mem_table = parser.get_table(
        re.compile(r'3. Memory'), 2)

    return {
        'lut_used':        int(logic_table[1][1]),
        'lut_avail':       int(logic_table[1][3]),
        'lut_logic_used':  int(logic_table[2][1]),
        'lut_logic_avail': int(logic_table[2][3]),
        'lut_mem_used':    int(logic_table[3][1]),
        'lut_mem_avail':   int(logic_table[3][3]),
        'reg_used':        int(logic_table[6][1]),
        'reg_avail':       int(logic_table[6][3]),
        'reg_ff_used':     int(logic_table[7][1]),
        'reg_ff_avail':    int(logic_table[7][3]),
        'reg_latch_used':  int(logic_table[8][1]),
        'reg_latch_avail': int(logic_table[8][3]),
        'bram_tile_used':  float(mem_table[1][1]),
        'bram_tile_avail': int(mem_table[1][3]),
        'bram36_used':     int(mem_table[2][1]),
        'bram36_avail':    int(mem_table[2][3]),
        'bram18_used':     int(mem_table[4][1]),
        'bram18_avail':    int(mem_table[4][3]),
    }
