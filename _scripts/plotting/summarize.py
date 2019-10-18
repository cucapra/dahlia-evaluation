#!/usr/bin/env python3

import sys

import summary as s

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Give the path to the results.json file to summarize.\n'
              'Usage: {} _results/<batch>/results.json'.format(sys.argv[0]),
              file=sys.stderr)
        sys.exit(1)

    template_params = {
        'unroll': 'results:dse_template:gemm.cpp:UNROLL'
    }

    fields = { **s.DEFAULT_KEYS,  **template_params, **s.UTIL_KEYS, }


    s.summarize(sys.argv[1], fields)
