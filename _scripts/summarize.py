#!/usr/bin/env python3

import json
import csv
import sys
import os

OUT_CSV = 'summary.csv'
FIELDS = [
    'bench',
    'version',
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


def summarize_one(job_results):
    bench_version, bench_name = job_results['bench'].split('/')

    return {
        # Identify the job.
        'bench': bench_name,
        'version': bench_version,
        'estimate': job_results['job']['config']['estimate'],

        # The results themselves.
        'perf_bram': job_results['results']['perf']['bram_used'],
        'synth_bram': job_results['results']['synthesis']['bram_used'],

        'perf_dsp': job_results['results']['perf']['dsp_used'],
        'synth_dsp': job_results['results']['synthesis']['dsp48_used'],

        'perf_ff': job_results['results']['perf']['ff_used'],
        'synth_ff': job_results['results']['synthesis']['ff_used'],

        'perf_lut': job_results['results']['perf']['lut_used'],
        'synth_lut': job_results['results']['synthesis']['lut_used'],

        'perf_lat': job_results['results']['perf']['hw_latency'],
        'synth_lat_min': job_results['results']['synthesis']['min_latency'],
        'synth_lat_max': job_results['results']['synthesis']['max_latency'],
    }


def summarize(results_json):
    # Load JSON data.
    with open(results_json) as f:
        results_data = json.load(f)

    # Generate summaries.
    out = [summarize_one(j) for j in results_data.values()]
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
