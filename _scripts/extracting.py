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
