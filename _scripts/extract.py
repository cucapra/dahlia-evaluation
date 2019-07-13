#!/usr/bin/env python3.7

import subprocess
import os
import sys
import logging
import json

import common
import extracting

BUILDBOT_URL = 'http://gorgonzola.cs.cornell.edu:8000'
DATA_COLLECTION = [
    {
        'file': '_sds/est/perf.est',
        'collect': extracting.performance_estimates,
    },
]

RESULTS_FILE = "results.json"  # Final, aggregated results for the batch.
DOWNLOAD_DIR = "raw"  # Subdirectory where we download files for extraction.
FAILURE_FILE = "failure_extract.txt"  # Job IDs we could not extract.


def get_metadata(job_id):
    """Download the configuration JSON located at $BUILDBOT/jobs/ and
    extracts metadata from it. Return None if the download fails.
    """
    # Download the json information form job page. Capture the Json
    res = subprocess.run(
        ['curl', '-sSf', '{}/jobs/{}'.format(common.buildbot_url(), job_id)],
        capture_output=True)

    if res.returncode == 0:
        json_rep = json.loads(res.stdout)
        return {
            'state': json_rep['state'],
            'hwname': json_rep['config']['hwname'],
        }
    else:
        return None


def download_files(base_dir, job_id, file_config):
    """Download specified files for the job from
    $BUILDBOT/jobs/<job_id>/files/code/<files>.
    to base_dir/job_id and runs the 'collect' method for each file.

    Return an object with the field 'success' and 'data'.
    'success' is False is there was at least one failure. 'data' contains the
    result of calling 'collect' on the filepath.
    """
    ret_obj = {"success": True, "data": []}

    # Create the job download directory
    job_path = os.path.join(base_dir, job_id)
    os.makedirs(job_path, exist_ok=True)

    logging.info("Extracting files for {} in {}".format(job_id, job_path))

    for conf in file_config:
        url = '{}/jobs/{}/files/code/{}'.format(
            common.buildbot_url(),
            job_id,
            conf['file'],
        )
        local_name = os.path.join(job_path, os.path.basename(conf['file']))
        cmd = ['curl', '-sSf', '-o', local_name, url]
        res = subprocess.run(cmd, capture_output=True)

        if res.returncode:
            ret_obj['success'] = False

            logging.error(
                'Failed to download {} for job {} with error {}'.format(
                    conf['file'], job_id, res.stderr.decode('utf-8')))
        else:
            ret_obj['data'].append(conf['collect'](local_name))

    return ret_obj


def extract_data(batch_dir):
    # Load the list of job IDs for this batch.
    job_file = os.path.join(batch_dir, common.JOBS_FILE)
    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))
    with open(job_file, "r") as jobs:
        job_ids = jobs.read().strip().split('\n')

    failed_jobs = set()
    json_data = []

    for job_id in job_ids:
        logging.info('Extracting %s', job_id)
        meta = get_metadata(job_id)
        if not meta:
            logging.error('Could not get information for %s.', job_id)
            failed_jobs.add(job_id)
        elif meta['state'] != 'done':
            logging.error('Job %s in state `%s`.', job_id, meta['state'])
            failed_jobs.add(job_id)
        else:
            hwname = meta['hwname']
            rptname = os.path.basename(hwname).split("-")[1]
            sds_report = '_sds/reports/sds_{}.rpt'.format(rptname)
            rpt_list = DATA_COLLECTION + [{
                'file': sds_report,
                'collect': extracting.synthesis_report,
            }]
            res = download_files(
                os.path.join(batch_dir, DOWNLOAD_DIR),
                job_id, rpt_list,
            )
            if not res['success']:
                failed_jobs.add(job_id)
            else:
                data = {
                    'bench': hwname,
                    'job_id': job_id,
                }

                # Include the output data from each extractor.
                for part in res['data']:
                    data.update(part)

                json_data.append(data)

    # Log failed jobs to failure.
    if failed_jobs:
        logging.warning('Failed jobs: %s', ', '.join(failed_jobs))
        with open(os.path.join(batch_dir, FAILURE_FILE), 'w') as failed:
            for job_id in failed_jobs:
                print(job_id, file=failed)

    # Write out results.
    report_path = os.path.join(batch_dir, RESULTS_FILE)
    with open(report_path, 'w') as report:
        logging.info("Writing data to {}".format(RESULTS_FILE))
        json.dump(json_data, report, sort_keys=True, indent=2)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Script requires a path to a directory for the batch.\n'
              'Usage: ./extract.py <batch-dir>')
        sys.exit(1)

    common.logging_setup()
    extract_data(sys.argv[1])
