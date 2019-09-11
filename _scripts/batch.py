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

# POST fields to pass to the Polyphemus during submission.
OPTIONS = {
    'make': '1',
}

def validate_conf(conf):
    """Validate a configuration that is going to be send to polyphemus.
    """

    # Required keys
    assert all (key in conf for key in ['file', 'hwname', 'estimate', 'make', 'skipexec']), "Required key is missing"

    # If estimate is true, skipexec cannot be true
    assert not (conf['estimate'] == True and conf['skipexec'] == False), "Estimate and skipexec cannot both be true"

    return conf


def submit(bench, pretend, conf):
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
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                check=True,
            )

            # Build up the job submission options.
            options = dict(OPTIONS)
            print(conf)
            options.update({
                'file': '@{}'.format(archive_name),
                'hwname': bench,
                'estimate': conf['estimate'],
                'skipexec': conf['skipexec'],
            })

            validate_conf(options)

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
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
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
            if not pretend:
                os.unlink(archive_name)

        return job_id


def batch_and_upload(benchmark_paths, pretend, conf):
    """Submit a batch of jobs to the Buildbot.
    """
    job_ids = []
    failed_paths = []

    # Submit all the benchmarks in this batch.
    for bench in benchmark_paths:
        logging.info('Submitting %s', bench)
        job_id = submit(bench, pretend, conf)
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

    # Dry run
    parser.add_argument('-p', '--pretend',
                        help="just print jobs (don't submit anything)",
                        action='store_true', dest='pretend')

    # Configuration for this set of jobs.
    parser.add_argument('-E', '--no-estimate',
                        help='full synthesis (not estimation)',
                        action='store_const', const='0', default='1', dest='estimate')
    parser.add_argument('-s', '--skip-exec',
                        help='Skip execution stage',
                        action='store_const', const='1', default='0', dest='skipexec')

    # Paths pointing to benchmarks to run.
    parser.add_argument('bench', nargs='+')

    opts = parser.parse_args()
    common.logging_setup()

    conf = {
        'estimate': opts.estimate,
        'skipexec': opts.skipexec,
    }

    batch_and_upload(opts.bench, opts.pretend, conf)
