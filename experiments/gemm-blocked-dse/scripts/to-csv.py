#!/usr/bin/env python3

import logging
import sys
import os
import json

from benchmarking import common
from benchmarking.summary import summarize


paths = {
    'bench': 'bench',
    'bank11':'gemm.cpp:BANK11',
    'bank12':'gemm.cpp:BANK12',
    'bank21':'gemm.cpp:BANK21',
    'bank22':'gemm.cpp:BANK22',
    'unroll1':'gemm.cpp:UNROLL1',
    'unroll2':'gemm.cpp:UNROLL2',
    'unroll3':'gemm.cpp:UNROLL3',
    'target_clock': 'target_clock',
    'estimated_clock':     'estimated_clock',
    'min_latency':     'min_latency',
    'max_latency':     'max_latency',
    'bram_used':     'bram_used',
    'dsp48_used':     'dsp48_used',
    'ff_used':     'ff_used',
    'lut_used':     'lut_used',
    'init_loop_lat':     'init_loop_lat',
    'compute_loop_lat':     'compute_loop_lat',
    'writeback_loop_lat':     'writeback_loop_lat',

}

if __name__ == '__main__':

    if len(sys.argv) != 2:
        print('Missing file name')
        sys.exit(1)

    common.logging_setup()

    with open(sys.argv[1]) as file:
        job = json.load(file)

        try:
            row = [ summarize.access_obj(job, path) for _, path in paths.items() ]
            print(*row, sep=',')
        except Exception as e:
            print(e)
            logging.error('Failed to get fields for {}'.format(sys.argv[1]))

