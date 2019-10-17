import re
import csv
import json
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


def _find_row(table, key, value):
    """Given a list of dicts in `table`, find the dict whose value for
    `key` contains `value`.
    """
    for row in table:
        if value in row[key]:
            return row
    raise ValueError("row not found")


def hls_report(filepath):
    """Parse a file of the format:
    http://cerberus.cs.cornell.edu:5000/jobs/F5sSE_bfhVQ/files/code/_x/gemm.hw.xilinx_aws-vu9p-f1-04261818_dynamic_5_0/gemm/gemm/solution/syn/report/gemm_csynth.rpt
    """
    parser = RPTParser(filepath)

    timing_table = parser.get_table(
        re.compile(r'\+ Timing \(ns\)'), 1)
    latency_table = parser.get_table(
        re.compile(r'\+ Latency \(clock cycles\)'), 1, True)
    utilization_table = parser.get_table(
        re.compile(r'== Utilization Estimates'), 2)

    # Get the (only) row in the table whose "Name" is "Total".
    util_total = _find_row(utilization_table, 'Name', 'Total')

    # Extract relevant data.
    return {
        'target_clock':    float(timing_table[0]['Target']),
        'estimated_clock': float(timing_table[0]['Estimated']),
        'min_latency':     int(latency_table[0]['Latency min']) if latency_table[0]['Latency min'].isdigit()  else None,
        'max_latency':     int(latency_table[0]['Latency max']) if latency_table[0]['Latency max'].isdigit()  else None,
        'pipelining':      latency_table[0]['Pipeline Type'],
        'bram_used':       int(util_total['BRAM_18K']),
        'dsp48_used':      int(util_total['DSP48E']),
        'ff_used':         int(util_total['FF']),
        'lut_used':        int(util_total['LUT']),
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


def get_number(string):
    """Given a string of the form 'dddd [xxxx]', return the string 'dddd'
    """
    m = re.search(r'\s*(\d+).*', string)
    assert m and m[1], 'Failed to cleanup {}.'.format(string)
    return m[1]


def util_routed_report(filepath):
    """Extract file of the form:
    http://cerberus.cs.cornell.edu:5000/jobs/F5sSE_bfhVQ/files/code/_x/reports/link/imp/kernel_util_routed.rpt
    """
    parser = RPTParser(filepath)

    util_table = parser.get_table(
        re.compile(r'.*SDx System Utilization'), 2)

    # Row for resource usages
    usages = _find_row(util_table, 'Name', 'Used Resources')
    usage_counts = { k.lower() + '_used': get_number(v) for k, v in usages.items() if k.lower() != 'name' }

    # Row for available resources
    avail = _find_row(util_table, 'Name', 'User Budget')
    avail_counts = { k.lower() + '_avail': get_number(v) for k, v in avail.items() if k.lower() != 'name' }

    return { **avail_counts, **usage_counts }


def runtime_log(filepath):
    """Extract runtime logs of the form:
    http://cerberus.cs.cornell.edu:5000/jobs/F5sSE_bfhVQ/files/code/runtime.log
    """
    fieldnames = ('iteration', 'time(ms)')
    with open(filepath) as f:
        csvfile = csv.DictReader(f, fieldnames, delimiter=',')
        return list(csvfile)
