#!/usr/bin/env python3.7

import subprocess
import os
import sys
import logging

import common

BUILDBOT_JOBS_URL='http://gorgonzola.cs.cornell.edu:8000/jobs'

# Written to when at least one benchmark upload fails.
FAILED_JOBS="failure.txt"

# Run make command to submit jobs in Buildbot
def batch_and_upload(benchmark_paths):
    cur_path = os.getcwd()
    job_ids = []
    failed_paths = []

    for bench in benchmark_paths:
        filepath = os.path.join(cur_path, bench)

        # Switch to benchmark directory and create a zip file
        with common.chdir(filepath):
            # Create zip with all the code.
            archive_name = os.path.basename(os.getcwd()) + ".zip"
            try:
                create_arch = subprocess.run(
                    ['zip', '-r', archive_name, '.'],
                    capture_output=True,
                    check=True)

                upload_cmd = ['curl', '-sS',
                              '-F', 'file=@{}'.format(archive_name),
                              '-F', 'skipseashell=1',
                              '-F', 'make=1',
                              BUILDBOT_JOBS_URL]

                # Upload the zip file to buildbot
                upload = subprocess.run(upload_cmd, capture_output=True, check=True)

                # Record the job id.
                job_ids.append(upload.stdout.decode('utf-8'))

                # Cleanup zip
                subprocess.run(['rm', archive_name])

            except subprocess.CalledProcessError as err:
                failed_paths.append(bench)
                logging.error(
                    'Command `' + ' '.join(err.cmd) + '` failed with:\n' +
                    err.stderr.decode('utf-8'))
                logging.warning('Ignoring benchmark ' + filepath)

    # Print all resulting job ids on stdout.
    for job_id in job_ids:
        print(job_id)

    if failed_paths:
        logging.warning(
            "Some benchmarks failed. Creating {} with failed paths".format(FAILED_JOBS))
        with open(FAILED_JOBS, 'w') as failed:
            for path in failed_paths:
                print(path, file=failed)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("No benchmark paths provided." +
              "\nUsage: ./batch.py <path-to-bench1> <path-to-bench2> ...")
        sys.exit(1)

    common.logging_setup()
    batch_and_upload(sys.argv[1:])
