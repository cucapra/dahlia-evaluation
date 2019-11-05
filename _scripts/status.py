#!/usr/bin/env python3

import os
import sys
import requests
from collections import defaultdict
import logging

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
        if info is None:
            logging.log(logging.ERROR,
                        '%s: Metadata Not Found',
                        job_id)
            continue;

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


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Script requires a path to a directory for the batch.\n'
              'Usage: ./status.py <batch-dir>')
        sys.exit(1)

    common.logging_setup()
    get_status(sys.argv[1])
