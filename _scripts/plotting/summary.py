import json
import csv
import sys
import os
import numpy as np

from collections import defaultdict

OUT_CSV = 'summary.csv'

HLS_KEYS = {
    'hls_lat_min':  'results:hls:min_latency' ,
    'hls_lat_max':  'results:hls:max_latency',
    'hls_lut':      'results:hls:lut_used',
    'hls_ff':       'results:hls:ff_used',
    'hls_bram':     'results:hls:bram_used',
    'hls_dsp':      'results:hls:dsp48_used',

}

EST_KEYS = {
    'est_lat':  'results:est:hw_latency' ,
    'est_lut':  'results:est:lut_used',
    'est_ff':   'results:est:ff_used',
    'est_bram': 'results:est:bram_used',
    'est_dsp':  'results:est:dsp_used',
}

FULL_KEYS = {
    'full_lut':     'results:full:lut_used',
    'full_ff':      'results:full:reg_ff_used',
    'full_bram':    'results:full:dsp_used',
    'full_dsp':     'results:full:dsp_used',
}

SDACCEL_CONF = {
    'target_freq': 'job:config:make_conf:TARGET_FREQ'
}

UTIL_KEYS = {
    "bram_used":    "results:routed_util:bram_used",
    "dsp_used":     "results:routed_util:dsp_used",
    "lut_used":     "results:routed_util:lut_used",
    "lutmem_used":  "results:routed_util:lutmem_used",
    "reg_used":     "results:routed_util:reg_used",
    "uram_used":    "results:routed_util:uram_used",
}

DEFAULT_KEYS = {
    'bench': 'bench',
}


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

def summarize_one(job_results, field_map):
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

    runtime = get_runtime(job_results)

    return { **out, **fields, **runtime }


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


def summarize(results_json, field_map):
    # Load JSON data.
    with open(results_json) as f:
        results_data = json.load(f)

    # Generate summaries.
    out = [summarize_one(j, field_map) for j in results_data.values()]

    # Sort to group benchmarks together.
    out.sort(key=lambda r: r['bench'])

    # Dump CSV output.
    headers = [f for f in out[0].keys()]
    csv_filename = os.path.join(os.path.dirname(results_json), OUT_CSV)
    with open(csv_filename, 'w') as f:
        writer = csv.DictWriter(f, headers)
        writer.writeheader()
        for res in out:
            writer.writerow(defaultdict(str, res))  # Silently allow blanks.
