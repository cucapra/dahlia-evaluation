#!/usr/bin/env python3

import json
import csv
import sys
import os
from collections import defaultdict

OUT_CSV = 'summary.csv'
FIELDS = [
    'bench',
    'version',
    'status',
    'estimate',
    'hls_lut',
    'est_lut',
    'full_lut',
    'hls_lat_min',
    'hls_lat_max',
    'est_lat',
    'hls_bram',
    'est_bram',
    'full_bram18',
    'full_bram36',
    'hls_dsp',
    'est_dsp',
    'full_dsp',
    'hls_ff',
    'est_ff',
    'full_ff',
]
BENCH_PREFIXES = ['machsuite-']  # Remove these prefixes for display.


def summarize_one(job_results):
    bench = job_results['bench']
    if bench.count('/') == 1:
        bench_version, bench_name = job_results['bench'].split('/')
    else:
        bench_version = ''
        bench_name = bench

    for prefix in BENCH_PREFIXES:
        if bench_name.startswith(prefix):
            bench_name = bench_name[len(prefix):]

    out = {
        # Identify the job.
        'bench': bench_name,
        'version': bench_version,
        'status': 'ok' if job_results['ok'] else 'error',

        # Configuration for the job.
        'estimate': job_results['job']['config']['estimate'],
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

    # Results from estimation.
    if 'est' in job_results['results']:
        est = job_results['results']['est']
        out.update({
            'est_bram': est['bram_used'],
            'est_dsp': est['dsp_used'],
            'est_ff': est['ff_used'],
            'est_lut': est['lut_used'],
            'est_lat': est['hw_latency'],
        })

    # Results from HLS compilation.
    if 'hls' in job_results['results']:
        hls = job_results['results']['hls']
        out.update({
            'hls_bram': hls['bram_used'],
            'hls_dsp': hls['dsp48_used'],
            'hls_ff': hls['ff_used'],
            'hls_lut': hls['lut_used'],
            'hls_lat_min': hls['min_latency'],
            'hls_lat_max': hls['max_latency'],
        })

    # Results from full synthesis.
    if 'full' in job_results['results']:
        full = job_results['results']['full']
        out.update({
            'full_bram18': full['bram18_used'],
            'full_bram36': full['bram36_used'],
            'full_lut': full['lut_used'],
            'full_ff': full['reg_ff_used'],
            'full_dsp': full['dsp_used'],
        })

    return out


def find_missing(summaries, ref_version, seeking_version):
    """Given a list of result summaries, find all the benchmarks that
    have an entry for `ref_version` but *not* for `seeking_version`.
    """
    ref_benchmaks = {s['bench'] for s in summaries
                     if s['version'] == ref_version}
    seeking_benchmarks = {s['bench'] for s in summaries
                          if s['version'] == seeking_version}
    return ref_benchmaks - seeking_benchmarks


def summarize(results_json):
    # Load JSON data.
    with open(results_json) as f:
        results_data = json.load(f)

    # Generate summaries.
    out = [summarize_one(j) for j in results_data.values()]

    # Insert "blank" lines for missing benchmark versions.
    for missing in find_missing(out, 'baseline', 'rewrite'):
        out.append({
            'bench': missing,
            'version': 'rewrite',
            'status': 'missing',
        })

    # Sort to group benchmarks together.
    out.sort(key=lambda r: (r['bench'], r['version']))

    # Dump CSV output.
    headers = [f for f in FIELDS if any(f in d for d in out)]
    csv_filename = os.path.join(os.path.dirname(results_json), OUT_CSV)
    with open(csv_filename, 'w') as f:
        writer = csv.DictWriter(f, headers)
        writer.writeheader()
        for res in out:
            writer.writerow(defaultdict(str, res))  # Silently allow blanks.


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Give the path to the results.json file to summarize.\n'
              'Usage: {} _results/<batch>/results.json'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)

    summarize(sys.argv[1])
