import os
import sys
import logging
import json

from benchmarking import common
from . import collect_configs

def job_json(batch_dir, job_id, collects):
    """Generate json summary for a single job.
    """

    return {}


def batch_json(batch_dir, static_collects, dyn_collects):
    """Generate a combined JSON summary for the batch.
    """

    job_file = os.path.join(batch_dir, common.JOBS_FILE)
    if not os.path.isfile(job_file):
        raise Exception("{} is not a file".format(job_file))
    with open(job_file, "r") as jobs:
        job_ids = jobs.read().strip().split('\n')

    failed_jobs = set()
