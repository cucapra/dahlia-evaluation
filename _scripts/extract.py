#!/usr/bin/env python3

import os
import sys
import logging
import json
import requests

import collect_configs
import common

RESULTS_FILE = "results.json"  # Final, aggregated results for the batch.
DOWNLOAD_DIR = "raw"  # Subdirectory where we download files for extraction.
FAILURE_FILE = "failure_extract.txt"  # Job IDs we could not extract.

# The list of files to download and extract.
STATIC_COLLECTIONS = [
    collect_configs.COLLECT_UTIL_ROUTED,
    collect_configs.COLLECT_RUNTIME,
    collect_configs.COLLECT_DSE_TEMPLATE,
]
DYN_COLLECTIONS = [collect_configs.COLLECT_HLS]


def _download(url, path, chunk_size=4096):
    """Download the file at `url` to `path`.
    """
    with requests.get(url, stream=True) as res:
        res.raise_for_status()
        with open(path, 'wb') as f:
            for chunk in res.iter_content(chunk_size=chunk_size):
                if chunk:
                    f.write(chunk)


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


def download_files(base_dir, job_id, file_config):
    """Download specified files for the job from
    $BUILDBOT/jobs/<job_id>/files/<files>.
    to base_dir/job_id and runs the 'collect' method for each file.

    Return a Boolean success flag (which is false if there was at least
    one failure) and a dict of outputs (the result of calling `collect` on
    each file path).
    """
    success = True
    out_data = {}

    # Create the job download directory
    job_path = os.path.join(base_dir, job_id)
    os.makedirs(job_path, exist_ok=True)

    logging.info(
        "Extracting files for {} in {} for collections {}.".format(
            job_id,
            job_path,
            ', '.join(map(lambda conf: conf.key, file_config))))

    for conf in file_config:
        url = '{}/jobs/{}/files/{}'.format(
            common.buildbot_url(),
            job_id,
            conf.file,
        )
        local_name = os.path.join(job_path, os.path.basename(conf.file))
        try:
            _download(url, local_name)
        except requests.HTTPError as exc:
            success = False
            logging.error(
                'Failed to download {} for job {}: {}'.format(
                    conf.file, job_id, exc))
        else:
            try:
                out_data[conf.key] = conf.collect(local_name)
            except Exception as err:
                success = False
                logging.error(
                    'Failed to parse {} for job {}.'.format(
                        conf.file, job_id))
                logging.error(err)

    return success, out_data


def extract_job(batch_dir, job_id):
    """Extract information about a single job. Return a dict of
    information to include about it. The dict *at least* has an 'ok'
    attribute, indicating whether the job is completely ready.
    """
    job = get_metadata(job_id)
    if not job:
        logging.error('Could not get information for %s.', job_id)
        return {'ok': False, 'error': 'job lookup failed'}

    hwname = job['config']['hwname']
    out = {
        'ok': True,
        'job': job,
        'bench': hwname,
    }

    if job['state'] != 'done':
        logging.error('Job %s in state `%s`.', job_id, job['state'])
        out.update({
            'ok': False,
            'error': 'job in state {}'.format(job['state']),
        })

    # Collections for this job
    collections = list(STATIC_COLLECTIONS)
    dynamic_collections = list(DYN_COLLECTIONS)

    # Get the list of files for the job.
    files_url = '{}/jobs/{}/files'.format(common.buildbot_url(), job_id)
    with requests.get(files_url) as res:
        res.raise_for_status()
        file_list = res.json()

    # Find paths for dynamic collections.
    for file_path in file_list:
        for idx, collect in enumerate(dynamic_collections):
            if not isinstance(collect.file, str) and collect.file(file_path):
                dynamic_collections.pop(idx)
                collections.append(collect_configs.CollectConfig(
                    key=collect.key,
                    file=collect.file(file_path),
                    collect=collect.collect,
                ))

    # Errors for dynamic_collections that didn't find matching file_paths.
    for collect in dynamic_collections:
        logging.error("Failed to find file for collection '%s' in job %s", collect.key, job_id)

    # Download files and extract results.
    success, res_data = download_files(
        os.path.join(batch_dir, DOWNLOAD_DIR),
        job_id,
        collections,
    )

    out['results'] = res_data
    if out['ok'] and not success:
        logging.error('Some data extraction failed.')
        out.update({
            'ok': False,
            'error': 'data extraction failed',
        })

    return out


def extract_batch(batch_dir):
    # Load the list of job IDs for this batch.
    job_file = os.path.join(batch_dir, common.JOBS_FILE)
    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))
    with open(job_file, "r") as jobs:
        job_ids = jobs.read().strip().split('\n')

    failed_jobs = set()
    json_data = {}

    for job_id in job_ids:
        logging.info('Extracting %s', job_id)
        data = extract_job(batch_dir, job_id)
        json_data[job_id] = data
        if not data['ok']:
            failed_jobs.add(job_id)

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
    extract_batch(sys.argv[1])
