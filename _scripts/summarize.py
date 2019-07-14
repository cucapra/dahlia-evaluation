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
    'perf_lut',
    'synth_lut',
    'perf_lat',
    'synth_lat_min',
    'synth_lat_max',
    'perf_bram',
    'synth_bram',
    'perf_dsp',
    'synth_dsp',
    'perf_ff',
    'synth_ff',
]
BENCH_PREFIXES = ['machsuite-']  # Remove these prefixes for display.


def summarize_one(job_results):
    bench_version, bench_name = job_results['bench'].split('/')
    for prefix in BENCH_PREFIXES:
        if bench_name.startswith(prefix):
            bench_name = bench_name[len(prefix):]

    return {
        # Identify the job.
        'bench': bench_name,
        'version': bench_version,
        'status': 'ok' if job_results['ok'] else 'error',

        # Configuration for the job.
        'estimate': job_results['job']['config']['estimate'],

        # The results themselves.
        'perf_bram': job_results['results']['perf']['bram_used'],
        'perf_dsp': job_results['results']['perf']['dsp_used'],
        'perf_ff': job_results['results']['perf']['ff_used'],
        'perf_lut': job_results['results']['perf']['lut_used'],
        'perf_lat': job_results['results']['perf']['hw_latency'],

        'synth_bram': job_results['results']['synthesis']['bram_used'],
        'synth_dsp': job_results['results']['synthesis']['dsp48_used'],
        'synth_ff': job_results['results']['synthesis']['ff_used'],
        'synth_lut': job_results['results']['synthesis']['lut_used'],
        'synth_lat_min': job_results['results']['synthesis']['min_latency'],
        'synth_lat_max': job_results['results']['synthesis']['max_latency'],
    }


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
        out.append(defaultdict(str, {
            'bench': missing,
            'version': 'rewrite',
            'status': 'missing',
        }))

    # Sort to group benchmarks together.
    out.sort(key=lambda r: (r['bench'], r['version']))

    # Dump CSV output.
    csv_filename = os.path.join(os.path.dirname(results_json), OUT_CSV)
    with open(csv_filename, 'w') as f:
        writer = csv.DictWriter(f, FIELDS)
        writer.writeheader()
        for res in out:
            writer.writerow(res)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Give the path to the results.json file to summarize.\n'
              'Usage: {} _results/<batch>/results.json'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)

    summarize(sys.argv[1])
