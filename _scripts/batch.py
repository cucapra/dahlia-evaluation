#!/usr/bin/env python3.7

import subprocess
import os
import sys
import logging

import common

BUILDBOT_JOBS_URL = 'http://gorgonzola.cs.cornell.edu:8000/jobs'

# Written to when at least one benchmark upload fails.
FAILED_JOBS = "failure_batch.txt"


def batch_and_upload(benchmark_paths):
    """Submit a batch of jobs to the Buildbot.
    """
    job_ids = []
    failed_paths = []

    for bench in benchmark_paths:
        logging.info('Submitting %s', bench)

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
                failed_paths.append(bench)
                logging.error(
                    'Submission for %s failed: `%s` produced:\n%s',
                    bench,
                    ' '.join(err.cmd),
                    err.stderr.decode('utf-8'),
                )

            else:
                # Record the job id.
                job_id = upload.stdout.decode('utf-8').strip()
                job_ids.append(job_id)

                # Clean up the zip file.
                os.unlink(archive_name)

                logging.info('Submitted %s', job_id)

    # Print all resulting job ids on stdout.
    for job_id in job_ids:
        print(job_id)

    if failed_paths:
        logging.warning(
            "Some benchmarks failed. Creating {} with "
            "failed paths".format(FAILED_JOBS)
        )
        with open(FAILED_JOBS, 'w') as failed:
            for path in failed_paths:
                print(path, file=failed)


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("No benchmark paths provided.\n"
              "Usage: ./batch.py <path-to-bench1> <path-to-bench2> ...")
        sys.exit(1)

    common.logging_setup()
    batch_and_upload(sys.argv[1:])
