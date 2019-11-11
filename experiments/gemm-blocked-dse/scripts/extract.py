#!/usr/bin/env python3

import json
import logging
import os
import re
import sys

from benchmarking import common
from benchmarking.summary import extracting
from benchmarking.parsing import rpt

def estimate_file(filepath):
    parser = rpt.RPTParser(filepath)

    timing_table = parser.get_table(
        re.compile(r'\+ Timing \(ns\)'), 1)
    latency_table = parser.get_table(
        re.compile(r'\+ Latency \(clock cycles\)'), 1, True)

    utilization_table = parser.get_table(
        re.compile(r'== Utilization Estimates'), 2)
    util_total = extracting._find_row(utilization_table, 'Name', 'Total')

    # Loop specific latencies
    loop_lat = parser.get_table(re.compile(r'\* Loop:'), 0, True)
    read_loop = extracting._find_row(loop_lat, 'Loop Name', '- Loop 1')
    compute_loop = extracting._find_row(loop_lat, 'Loop Name', '- Loop 2')
    write_loop = extracting._find_row(loop_lat, 'Loop Name', '- Loop 3')

    # Extract relevant data.
    return {
        'target_clock':    float(timing_table[0]['Target']),
        'estimated_clock': float(timing_table[0]['Estimated']),
        'min_latency':     int(latency_table[0]['Latency min']),
        'max_latency':     int(latency_table[0]['Latency max']),
        'bram_used':       int(util_total['BRAM_18K']),
        'dsp48_used':      int(util_total['DSP48E']),
        'ff_used':         int(util_total['FF']),
        'lut_used':        int(util_total['LUT']),
        'init_loop_lat':   read_loop['Latency min'],
        'compute_loop_lat': compute_loop['Latency min'],
        'writeback_loop_lat': write_loop['Latency min'],
    }


def get_metadata(filepath):
    json = extracting.json_report(filepath)

    return {
        'bench': json['config']['hwname'],
        'config': json['config']
    }

# Mapping from files to functions to run for them.
FILES = {
    "./info.json": get_metadata,
    "code/template.json": extracting.json_report,
    "code/_x/gemm.sw_emu.xilinx_aws-vu9p-f1-04261818_dynamic_5_0/gemm/gemm/solution/syn/report/gemm_csynth.rpt": estimate_file,
}

FOLDER = 'raw'


def get_job_data(job_name):
    """Extract data to raw/job_name.json given a `job_name`
    """
    out_data = {}
    for filename, collect in FILES.items():
        # try:
            out = collect(os.path.join(job_name, filename))
            for k, v in out.items():
                out_data[k] = v
        # except Exception as e:
            # print(e)
            # logging.error('Failed to extract {} from job {}'.format(filename, job_name))

    return out_data


def extract_job(job_name):
    if not os.path.isdir(FOLDER):
        os.mkdir(FOLDER)

    out = get_job_data(job_name)

    with open(os.path.join(FOLDER, job_name + '.json'), 'w') as file:
        json.dump(out, file, indent=2)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Missing job name')
        sys.exit(1)

    common.logging_setup()

    extract_job(sys.argv[1])
