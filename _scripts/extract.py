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
import extracting

BUILDBOT_URL = 'http://gorgonzola.cs.cornell.edu:8000'
DATA_COLLECTION = [
    { 'file': '_sds/est/perf.est', 'collect': extracting.performance_estimates },
]

# Written to when at least one job fails to extract
FAILURE = "failure_extract.txt"

# Store performance estimates
REPORT = "batch_data.csv"

def flatten(llst):
    return [val for lst in llst for val in lst]

def get_metadata(job_id):
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
        return { 'state': json_rep['state'], 'hwname': json_rep['config']['hwname'] }
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
    os.makedirs(job_path, exist_ok=True)

    logging.info("Extracting files for {} in {}".format(job_id, job_path))

    for conf in file_config:
        url = '{}/jobs/{}/files/code/{}'.format(BUILDBOT_URL, job_id, conf['file'])
        local_name = os.path.join(job_path, os.path.basename(conf['file']))
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

    csv_hdrs = None
    csv_data = []

    with open(job_file, "r+") as jobs:
        job_ids = jobs.read().split('\n')[:-1]

    # Create a temporary directory to store downloaded files
    batch_dir = os.path.join(os.getcwd(), 'extracted_data_' + job_ids[0])
    os.makedirs(batch_dir, exist_ok=True)
    logging.info("Create batchdir {}".format(batch_dir))

    for job_id in job_ids:
        meta = get_metadata(job_id)
        if not meta:
            logging.error(
                'Could not get information for {}. Skipping file download.'.format(job_id))
            failed_jobs.add(job_id)
        elif meta['state'] != 'done':
            logging.error(
                'Job {} in state {}. Skipping file download.'.format(job_id, meta['state']))
            failed_jobs.add(job_id)
        else:
            hwname = meta['hwname']
            rptname = os.path.basename(hwname).split("-")[1]
            sds_report = '_sds/reports/sds_{}.rpt'.format(rptname)
            rpt_list = DATA_COLLECTION + [
                    { 'file': sds_report, 'collect': extracting.synthesis_report }
            ]
            res = download_files(batch_dir, job_id, rpt_list)
            if not res['success']:
                failed_jobs.add(job_id)
            else:
                data = flatten(map(lambda d: d['data'], res['data']))
                hdrs = flatten(map(lambda d: d['hdr'], res['data']))
                if not csv_hdrs:
                    csv_hdrs = ['bench', 'job_id'] + hdrs

                csv_data.append([hwname, job_id] + data)

    # Log failed jobs to Faliure
    if failed_jobs:
        logging.warning('Some jobs failed. Writing job names to {}.'.format(FAILURE))
        with open(FAILURE, 'w') as failed:
            for job_id in failed_jobs:
                print(job_id, file=failed)

    # Write timing data to csv
    report_path = os.path.join(batch_dir, REPORT)
    with open(report_path, 'w') as report:
        logging.info("Writing estimate data to {}".format(REPORT))
        writer = csv.writer(report)
        writer.writerow(csv_hdrs)
        writer.writerows(csv_data)

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Script requires path to file containing jobs ids.\nUsage ./extract.py <jobs.txt>')
        sys.exit(1)

    common.logging_setup()
    extract_data(sys.argv[1])
