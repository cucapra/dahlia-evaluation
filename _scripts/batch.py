#!/usr/bin/env python3

import subprocess
import os
import sys
import logging
import datetime

import common

BUILDBOT_JOBS_URL = common.buildbot_url() + '/jobs'

# Written to when at least one benchmark upload fails.
FAILED_JOBS = "failure_batch.txt"


def submit(bench):
    """Given a path to a benchmark, submit it and return either a job ID
    or None if submission failed.
    """
    # Switch to benchmark directory to do the submission.
    with common.chdir(bench):
        try:
            # Create zip with all the code.
            archive_name = os.path.basename(os.getcwd()) + ".zip"
            subprocess.run(
                ['zip', '-r', archive_name, '.'],
                capture_output=True,
                check=True,
            )

            # Upload the zip file to Buildbot.
            upload_cmd = ['curl', '-sS',
                          '-F', 'file=@{}'.format(archive_name),
                          '-F', 'hwname={}'.format(bench),
                          '-F', 'estimate=1',
                          '-F', 'skipseashell=1',
                          '-F', 'make=1',
                          BUILDBOT_JOBS_URL]
            upload = subprocess.run(
                upload_cmd,
                capture_output=True,
                check=True,
            )

        except subprocess.CalledProcessError as err:
            logging.error(
                'Submission for %s failed: `%s` produced:\n%s',
                bench,
                ' '.join(err.cmd),
                err.stderr.decode('utf-8'),
            )
            return None

        else:
            # Clean up the zip file.
            os.unlink(archive_name)

            return upload.stdout.decode('utf-8').strip()


def batch_and_upload(benchmark_paths):
    """Submit a batch of jobs to the Buildbot.
    """
    job_ids = []
    failed_paths = []

    # Submit all the benchmarks in this batch.
    for bench in benchmark_paths:
        logging.info('Submitting %s', bench)
        job_id = submit(bench)
        if job_id:
            job_ids.append(job_id)
            logging.info('Submitted %s', job_id)
        else:
            failed_paths.append(bench)

    # Create a directory for the batch.
    batch_name = datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
    batch_dir = os.path.join(common.OUT_DIR, batch_name)
    os.makedirs(batch_dir, exist_ok=True)

    # The only thing we print to stdout is the name of the batch.
    print(batch_name)

    # Record the job IDs.
    with open(os.path.join(batch_dir, common.JOBS_FILE), 'w') as f:
        for job_id in job_ids:
            print(job_id, file=f)

    # Record any failures.
    if failed_paths:
        logging.warning('Submission failed for: %s', ', '.join(failed_paths))
        with open(os.path.join(batch_dir, FAILED_JOBS), 'w') as f:
            for path in failed_paths:
                print(path, file=f)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("No benchmark paths provided.\n"
              "Usage: ./batch.py <path-to-bench1> <path-to-bench2> ...")
        sys.exit(1)

    common.logging_setup()
    batch_and_upload(sys.argv[1:])
