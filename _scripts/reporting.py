import re
import os
import sys
import csv
import logging
import xml.etree.ElementTree as ET

from rpt import RPTParser

def performance_report(filepath):
    """
    perf.est files are xml files. We parse the file as XML data and extract
    the hwLatency field and some resource fields
    """
    resources = ['dsp', 'bram', 'lut', 'ff']
    resource_stats = ['used', 'total']
    headers = ['hw_latency']
    for resource in resources:
        for stat in resource_stats:
            headers.append("{}_{}".format(resource, stat))

    data = []

    with open(filepath) as file:
        src = file.read()

        root = ET.fromstring(src)

        # Extract hw_latency
        hw = ''
        graph = root.find('graph')
        if graph:
            hw = graph.attrib['hwLatency']

        data.append(hw)

        # Extract all the resources
        for resource in resources:
            res = root.find("./resources/resource[@name='{}']".format(resource))
            for resource_stat in resource_stats:
                resource_val = ""
                if res != None:
                    resource_val = res.attrib[resource_stat]
                data.append(resource_val)

        return {
            "hdr": headers,
            "data": data
        }

def synthesis_report(filepath):

    parser = RPTParser(filepath)

    timing_table = parser.get_table(
        re.compile(r'\+ Timing \(ns\)'), 1)
    latency_table = parser.get_table(
        re.compile(r'\+ Latency \(clock cycles\)'), 1, True)
    utilization_table = parser.get_table(
        re.compile(r'== Utilization Estimates'), 2)

    # Extract relevant data
    hdr = [
        'target_clock',
        'estimated_clock',
        'min_latency',
        'max_latency',
        'pipelining',
        'bram_used',
        'dsp48_used',
        'ff_used',
        'lut_used'
    ]

    data = [
        timing_table[-1][1],
        timing_table[-1][2],
        latency_table[-1][0],
        latency_table[-1][1],
        latency_table[-1][4],
        utilization_table[-3][1],
        utilization_table[-3][2],
        utilization_table[-3][3],
        utilization_table[-3][4]
    ]

    return {
        "hdr": hdr,
        "data": data
    }
