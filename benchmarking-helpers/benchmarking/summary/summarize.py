import json
import csv
import sys
import os
import argparse
import numpy as np

from collections import defaultdict, ChainMap

from . import summary_configs

def access_obj(obj, acc):
    """Given `acc` of the form `x:y:z`, access the field `obj.x.y.z`
    """
    accessors = acc.split(':')
    ret = obj
    while accessors and ret:
        ret = ret.get(accessors.pop(0))
    return ret


def get_runtime(job_results, runtime_key="time(ms)"):
    """Collect statistics from the runtime information array.
    """

    runtime_arr = [float(r[runtime_key]) for r in job_results['results']['runtime']
                   if r[runtime_key] != runtime_key]

    return {
        'runtime_avg': np.mean(runtime_arr),
        'runtime_std': np.std(runtime_arr),
    }

def summarize_one(job_results, field_map, runtime):
    bench = job_results['bench']

    out = {
        # Identify the job.
        'status': 'ok' if job_results['ok'] else 'error',
    }

    # Abort early if we don't have statistics.
    if 'results' not in job_results:
        return out

    # Try to provide a better status message.
    if 'log' in job_results['results']:
        log = job_results['results']['log']
        for key, present in log.items():
            if present:
                out['status'] = key
                break

    fields = {k: access_obj(job_results, v) for k, v in field_map.items()}

    if runtime:
        runtime_fields = get_runtime(job_results)
    else:
        runtime_fields = {}

    return { **out, **fields, **runtime_fields }


def find_missing(summaries, ref_version, seeking_version, field_map, key_name='version'):
    """Given a list of result summaries, find all the benchmarks that
    have an entry for `ref_version` but *not* for `seeking_version`
    for the given `key_name`.
    """
    ref_benchmaks = {s['bench'] for s in summaries
                     if s[key_name] == ref_version}
    seeking_benchmarks = {s['bench'] for s in summaries
                          if s[key_name] == seeking_version}
    return ref_benchmaks - seeking_benchmarks


def summarize_json(results_json, field_map, runtime, out_csv):
    # Load JSON data.
    with open(results_json) as f:
        results_data = json.load(f)

    # Generate summaries.
    out = [summarize_one(j, field_map, runtime) for j in results_data.values()]

    # Dump CSV output.
    headers = [f for f in out[0].keys()]
    csv_filename = os.path.join(os.path.dirname(results_json), out_csv)
    with open(csv_filename, 'w') as f:
        writer = csv.DictWriter(f, headers)
        writer.writeheader()
        for res in out:
            writer.writerow(defaultdict(str, res))  # Silently allow blanks.
