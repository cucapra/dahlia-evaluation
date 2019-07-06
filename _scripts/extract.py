#!/usr/bin/env python3.7

import subprocess
import re
import os
import sys
import tempfile
import logging
import json
import csv
import shutil

import common
import reporting

BUILDBOT_URL = 'http://gorgonzola.cs.cornell.edu:8000'
METADATA_FIELDS = ['state', 'hw_basename']

STATS_COLLECTION = [
    { 'file': '_sds/est/perf.est', 'collect': reporting.performance_report }
]

# Written to when at least one job fails to extract
FAILURE = "failure_extract.txt"

# Store the timing results
TIMING = "timing.csv"

def flatten(llst):
    return [val for lst in llst for val in lst]

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

def download_files(base_dir, job_id, file_config):
    """
    Downloads specified files for the job from BUILDBOT_URL/jobs/<job_id>/files/code/<files>.
    to base_dir/job_id and runs the 'collect' method for each file.

    Returns an object with the field 'success' and 'data'.
    'success' is False is there was at least one failure. 'data' contains the
    result of calling 'collect' on the filepath.
    """
    ret_obj = { "success": True, "data": [] }

    # Create the job download directory
    job_path = os.path.join(base_dir, job_id)
    os.mkdir(job_path)

    logging.info("Extracting files for {} in {}".format(job_id, job_path))

    for conf in file_config:
        url = '{}/jobs/{}/files/code/{}'.format(BUILDBOT_URL, job_id, conf['file'])
        local_file = re.sub('/', '-', conf['file'])
        local_name = os.path.join(job_path, local_file)
        cmd = ['curl', '-sSf', '-o', local_name, url]
        res = subprocess.run(cmd, capture_output=True)

        if res.returncode != 0:
            ret_obj['success'] = False

            logging.error(
                'Failed to download {} for job {} with error {}'.format(
                    conf['file'], job_id, res.stderr.decode('utf-8')))
        else:
            ret_obj['data'].append(conf['collect'](local_name))

    return ret_obj

def extract_data(job_file):
    job_ids = []
    failed_jobs = set()

    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))

    # Create a temporary directory to store downloaded files
    tmp_dir = tempfile.mkdtemp()
    logging.info("Create tmpdir {}".format(tmp_dir))

    csv_hdrs = None
    csv_data = []

    with open(job_file, "r+") as jobs:
        job_ids = jobs.read().split('\n')[:-1]

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
            hw_basename = meta['hw_basename']
            sds_report = '_sds/reports/sds_{}.rpt'.format(hw_basename)
            all_stats = STATS_COLLECTION + [
                {'file': sds_report, 'collect': reporting.synthesis_report }
            ]
            res = download_files(tmp_dir, job_id, all_stats)
            if not res['success']:
                failed_jobs.add(job_id)
            else:
                data = flatten(map(lambda d: d['data'], res['data']))
                hdrs = flatten(map(lambda d: d['hdr'], res['data']))
                if not csv_hdrs:
                    csv_hdrs = ['bench', 'job_id'] + hdrs

                csv_data.append([hw_basename, job_id] + data)

    # Log failed jobs to Faliure
    if failed_jobs:
        logging.warning('Some jobs failed. Writing job names to {}.'.format(FAILURE))
        with open(FAILURE, 'w') as failed:
            for job_id in failed_jobs:
                print(job_id, file=failed)
    # If everything went fine, delete the tmp directory
    else:
        shutil.rmtree(tmp_dir)

    # Write timing data to csv
    with open(TIMING, 'w') as timing:
        logging.info("Writing timing data to {}".format(TIMING))
        writer = csv.writer(timing)
        writer.writerow(csv_hdrs)
        writer.writerows(csv_data)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Script requires path to file containing jobs ids.\nUsage ./extract.py <jobs.txt>')
        sys.exit(1)

    common.logging_setup()
    extract_data(sys.argv[1])
