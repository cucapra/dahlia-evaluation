#!/usr/bin/env python3

import os
import sys
import logging
import json
import requests
import argparse

from benchmarking import common
from benchmarking.summary import collect_configs
from benchmarking.polyphemus import extract_batch


RESULTS_FILE = "results.json"  # Final, aggregated results for the batch.
DOWNLOAD_DIR = "raw"  # Subdirectory where we download files for extraction.
FAILURE_FILE = "failure_extract.txt"  # Job IDs we could not extract.

# The list of files to download and extract.
DEFAULT_COLLECTS = [
    'routed_util', 'runtime', 'dse_template', 'hls'
]

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Extract data from Cerberus jobs. Must be called called on a directory containing a jobs.txt file.')

    # Parse name for collect configurations.
    parser.add_argument('-c', '--collects', default=None, nargs="*",
                        help='Collect configurations to use for extraction. One of {}'.format(', '.join(collect_configs.VALID_COLLECTS.keys())))

    # Jobs directory
    parser.add_argument('jobs')

    opts = parser.parse_args()
    common.logging_setup()

    collect_strs = opts.collects or DEFAULT_COLLECTS
    collects = [ collect_configs.parse_collect(conf) for conf in collect_strs ]
    static_collects = [ conf for conf in collects if isinstance(conf.file, str) ]
    dyn_collects = [ conf for conf in collects if not isinstance(conf.file, str) ]

    extract_batch(sys.argv[1], static_collects, dyn_collects)
