#!/usr/bin/env python3

import subprocess
import os
import logging
import datetime
import argparse

import common

BUILDBOT_JOBS_URL = common.buildbot_url() + '/jobs'

# Written to when at least one benchmark upload fails.
FAILED_JOBS = "failure_batch.txt"

# POST fields to pass to the Buildbot during submission.
OPTIONS = {
    'skipseashell': '1',
    'make': '1',
}


def submit(bench, estimate, pretend):
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

            # Build up the job submission options.
            options = dict(OPTIONS)
            options.update({
                'file': '@{}'.format(archive_name),
                'hwname': bench,
            })
            if estimate:
                options['estimate'] = '1'
            else:
                options['skipexec'] = '1'

            # Upload the zip file to Buildbot.
            upload_cmd = ['curl', '-sS']
            upload_cmd += ['-F{}={}'.format(k, v) for k, v in options.items()]
            upload_cmd.append(BUILDBOT_JOBS_URL)
            if pretend:
                logging.info('Pretend submit: %s', upload_cmd)
                job_id = '(dummy id: {})'.format(bench)
            else:
                upload = subprocess.run(
                    upload_cmd,
                    capture_output=True,
                    check=True,
                )
                job_id = upload.stdout.decode('utf-8').strip()

        except subprocess.CalledProcessError as err:
            logging.error(
                'Submission for %s failed: `%s` produced:\n%s',
                bench,
                ' '.join(err.cmd),
                err.stderr.decode('utf-8'),
            )
            return None

        finally:
            # Clean up the zip file.
            os.unlink(archive_name)

        return job_id


def batch_and_upload(benchmark_paths, estimate, pretend):
    """Submit a batch of jobs to the Buildbot.
    """
    job_ids = []
    failed_paths = []

    # Submit all the benchmarks in this batch.
    for bench in benchmark_paths:
        logging.info('Submitting %s', bench)
        job_id = submit(bench, estimate, pretend)
        if job_id:
            job_ids.append(job_id)
            logging.info('Submitted %s', job_id)
        else:
            failed_paths.append(bench)

    # Choose a "name" for the batch and print it to stdout.
    batch_name = datetime.datetime.now().strftime('%Y-%m-%d-%H-%M-%S')
    print(batch_name)
    if pretend:
        return

    # Create a directory for the batch.
    batch_dir = os.path.join(common.OUT_DIR, batch_name)
    os.makedirs(batch_dir, exist_ok=True)

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
    parser = argparse.ArgumentParser(description='Submit Buildbot jobs.')
    parser.add_argument('-E', '--no-estimate',
                        help='full synthesis (not estimation)',
                        action='store_false', dest='estimate')
    parser.add_argument('-p', '--pretend',
                        help="just print jobs (don't submit anything)",
                        action='store_true', dest='pretend')
    parser.add_argument('bench', nargs='+')
    opts = parser.parse_args()

    common.logging_setup()
    batch_and_upload(opts.bench, opts.estimate, opts.pretend)
