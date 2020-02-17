#!/usr/bin/env python3

import logging
import sys
import os
import json

from benchmarking import common
from benchmarking.summary import summarize


paths = {
    'bench': 'bench',
    'target_clock': 'target_clock',
    'estimated_clock':     'estimated_clock',
    'min_latency':     'min_latency',
    'max_latency':     'max_latency',
    'bram_used':     'bram_used',
    'dsp48_used':     'dsp48_used',
    'ff_used':     'ff_used',
    'lut_used':     'lut_used',
    # 'init_loop_lat':     'init_loop_lat',
    # 'compute_loop_lat':     'compute_loop_lat',
    # 'writeback_loop_lat':     'writeback_loop_lat',

}

if __name__ == '__main__':

    if len(sys.argv) == 2:
        with open(sys.argv[1]) as file:
            more_keys = json.load(file)

        all_paths = {**more_keys, **paths}

        print(','.join(all_paths.keys()))
        sys.exit(0)


    if len(sys.argv) != 3:
        print('Missing file name or keys.json')
        sys.exit(1)

    common.logging_setup()

    with open(sys.argv[2]) as file:
        more_keys = json.load(file)

    all_paths = {**more_keys, **paths}

    with open(sys.argv[1]) as file:
        job = json.load(file)

        try:
            row = [ summarize.access_obj(job, path) for _, path in all_paths.items() ]
            print(*row, sep=',')
        except Exception as e:
            print(e)
            logging.error('Failed to get fields for {}'.format(sys.argv[1]))

