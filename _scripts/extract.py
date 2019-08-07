#!/usr/bin/env python3

import os
import sys
import logging
import json
import requests
import re

import common
import extracting

# Instructions for data collection and extraction. In each, `file` is
# the path to a file to download and analyze. `collect` is a function to
# call on the downloaded file, which should return JSON-serializable
# information to include in the results. `key` is the key to use in the
# result data.

# This one is the output of *estimation*, which runs on the complete
# design (not just a single kernel). It appears to happen *after* HLS.
# It only happens on estimation runs; not on full synthesis runs.
COLLECT_EST = {
    'key': 'est',
    'file': 'code/_sds/est/perf.est',
    'collect': extracting.performance_estimates,
}

# This one is the report from HLS-compiling the kernel source code to
# RTL. It seems to exist regardless of whether we're doing estimation. The
# filename here needs to be filled in by searching the file list.
COLLECT_HLS = {
    'key': 'hls',
    'file': None,
    'collect': extracting.hls_report,
}

# The *overall* report from synthesis. Only available on full runs (not
# estimation). Available in non-estimation SDSoC runs and Vivado HLS runs where
# the tool is configured to run full synthesis, not just HLS itself.
COLLECT_FULL = {
    'key': 'full',
    'file': 'code/_sds/reports/sds.rpt',
    'collect': extracting.sds_report,
}

# Just check for various messages in the job log.
COLLECT_LOG = {
    'key': 'log',
    'file': 'log.txt',
    'collect': extracting.log_messages,
}

RESULTS_FILE = "results.json"  # Final, aggregated results for the batch.
DOWNLOAD_DIR = "raw"  # Subdirectory where we download files for extraction.
FAILURE_FILE = "failure_extract.txt"  # Job IDs we could not extract.


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

    logging.info("Extracting files for {} in {}".format(job_id, job_path))

    for conf in file_config:
        url = '{}/jobs/{}/files/{}'.format(
            common.buildbot_url(),
            job_id,
            conf['file'],
        )
        local_name = os.path.join(job_path, os.path.basename(conf['file']))
        try:
            _download(url, local_name)
        except requests.HTTPError as exc:
            success = False
            logging.error(
                'Failed to download {} for job {}: {}'.format(
                    conf['file'], job_id, exc))
        else:
            out_data[conf['key']] = conf['collect'](local_name)

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

    # Get the list of files for the job.
    files_url = '{}/jobs/{}/files'.format(common.buildbot_url(), job_id)
    with requests.get(files_url) as res:
        res.raise_for_status()
        file_list = res.json()

    # The list of files to download and extract.
    collections = [COLLECT_LOG]

    # TODO Restore collection for SDSoC.
    # if estimate:
    #     collections += [COLLECT_EST]
    # else:
    #     collections += [COLLECT_FULL]

    # Find the name of the HLS report, which exists for both Vivado HLS jobs
    # and SDSoC jobs---but in different locations!
    for file_path in file_list:
        m = re.search(r'/reports/sds_(\w+).rpt', file_path)
        if m and m.groups(1) != 'main':
            hls_rpt_path = file_path
            break
        if re.search(r'/report/(\w+)_csynth.rpt', file_path):
            hls_rpt_path = file_path
            break
    else:
        logging.error('HLS report not found for job %s', job_id)
        hls_rpt_path = None

    # Collect the HLS report, if available.
    if hls_rpt_path:
        collect_hls = dict(COLLECT_HLS)
        collect_hls['file'] = hls_rpt_path
        collections.append(collect_hls)

    # In *non-estimate* Vivado HLS runs only, look for the full
    # synthesis report.
    # FIXME: Don't do this for SDSoC runs, wherein the report lives in
    # `sds.rpt`.
    if not job['estimate']:
        for file_path in file_list:
            if re.search(r'/verilog/report/(\w+)_utilization_synth.rpt',
                         file_path):
                synth_rpt_path = file_path
                break
        else:
            logging.error('Synthesis report not found for job %s', job_id)
            synth_rpt_path = None

        # Collect the HLS synthesis report, if available.
        if synth_rpt_path:
            collect_synth = dict(COLLECT_FULL)
            collect_synth['file'] = synth_rpt_path
            collections.append(collect_synth)

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
