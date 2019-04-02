#!/usr/bin/env python3.7

import subprocess
import re
import os
import sys
import tempfile
import logging
import json

import common

BUILDBOT_URL = 'http://gorgonzola.cs.cornell.edu:8000'
STATS_FILE = ['hwperf.csv', 'data_motion.csv', 'input.data']
METADATA_FIELDS = ['state', 'hw_basename']

def get_metadata(job_id, metadata_fields):
    """
    Downloads the configuration json located at BUILDBOT_URL/jobs/ and
    extracts metadata from it. Returns None if the download fails.
    """
    # Download the json information form job page. Capture the Json
    res = subprocess.run(
        ['curl', '-sSf', '{}/jobs/{}'.format(BUILDBOT_URL, job_id)],
        capture_output=True)

    if res.returncode == 0:
        json_rep = json.loads(res.stdout)
        return {field: json_rep[field] for field in metadata_fields}
    else:
        return None

def download_files(base_dir, job_id, files):
    """
    Downloads specified files for the job from BUILDBOT_URL/jobs/<job_id>/files/code/<files>.
    to base_dir/job_id.

    Returns an object with the field 'success' and 'files'.
    'success' is False is there was at least one failure. 'files' contains path
    to the downloaded files.
    """
    ret_obj = { "success": True, "files": [] }

    # Create the job download directory
    job_path = os.path.join(base_dir, job_id)
    os.mkdir(job_path)

    logging.info("Extracting files for {} in {}".format(job_id, job_path))

    for file in files:
        url = '{}/jobs/{}/files/code/{}'.format(BUILDBOT_URL, job_id, file)
        local_name = os.path.join(job_path, '{}_{}'.format(job_id, file))
        cmd = ['curl', '-sSf', '-o', local_name, url]
        res = subprocess.run(cmd, capture_output=True)

        if res.returncode != 0:
            ret_obj['success'] = False

            logging.error(
                'Failed to download {} for job {} with error {}'.format(
                    file, job_id, res.stderr.decode('utf-8')))
        else:
            ret_obj['files'].append(local_name)

    return ret_obj

def extract_data(job_file):
    job_ids = []
    failed_jobs = set()

    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))

    # Create a temporary directory to store downloaded files
    tmp_dir = tempfile.mkdtemp()
    logging.info("Create tmpdir {}".format(tmp_dir))

    with open(job_file, "r+") as jobs:
        for line in jobs:
            job_ids.append(line.rstrip()) # Remove newline character from JobID

    # TODO(rachit): Where are my monads??
    for job_id in job_ids:
        meta = get_metadata(job_id, METADATA_FIELDS)
        if not meta:
            logging.error(
                'Could not get information for {}. Skipping file download.'.format(job_id))
            failed_jobs.add(job_id)
        elif meta['state'] != 'done':
            logging.error(
                'Job {} in state {}. Skipping file download.'.format(job_id, meta['state']))
            failed_jobs.add(job_id)
        else:
            res = download_files(tmp_dir, job_id, STATS_FILE)
            if not res['success']:
                failed_jobs.add(job_id)

    if failed_jobs:
        logging.warning('Some jobs failed. Writing job names to STDOUT.')
        for job_id in failed_jobs:
            print(job_id)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Script requires path to file containing jobs ids.\nUsage ./extract.py <jobs.txt>')
        sys.exit(1)

    common.logging_setup()
    extract_data(sys.argv[1])
