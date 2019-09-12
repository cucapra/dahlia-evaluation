#!/usr/bin/env python3

import os
import sys
import requests
import logging
import argparse
import time
from subprocess import call
from collections import defaultdict

import common


def get_metadata(job_id):
    """Download the configuration JSON located at $BUILDBOT/jobs/.
    Return None if the download fails.
    """
    url = '{}/jobs/{}'.format(common.buildbot_url(), job_id)
    try:
        with requests.get(url) as res:
            res.raise_for_status()
            return res.json()
    except requests.HTTPError:
        return None


def get_status(batch_dir):
    # Load the list of job IDs for this batch.
    job_file = os.path.join(batch_dir, common.JOBS_FILE)
    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))
    with open(job_file, "r") as jobs:
        job_ids = jobs.read().strip().split('\n')

    # Download all the status stuff.
    job_info = {}
    for job_id in job_ids:
        info = get_metadata(job_id)
        logging.log(
            logging.ERROR if info['state'] == 'failed' else logging.INFO,
            '%s (%s): %s',
            job_id, info['config']['hwname'], info['state'],
        )
        job_info[job_id] = info

    # Count up the states.
    counts = defaultdict(int)
    for meta in job_info.values():
        counts[meta['state']] += 1

    # Print out a summary.
    for key, count in sorted(counts.items()):
        print('{}: {}/{}'.format(key, count, len(job_info)))

def get_status_rep(watch, batch_dir):
    if watch:
        while True:
            get_status(batch_dir)
            time.sleep(5)
            _ = call('clear' if os.name == 'posix' else 'cls')
    else:
        get_status(batch_dir)



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Show status of buildbot jobs.")

    # Run in watch mode
    parser.add_argument('-w', '--watch',
                        help='Keep running the command every 5s',
                        action='store_true', dest='watch')

    # Batch directory
    parser.add_argument('batch_dir', nargs=1)

    opts = parser.parse_args()

    common.logging_setup()
    get_status_rep(opts.watch, opts.batch_dir[0])
