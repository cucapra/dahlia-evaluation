#!/usr/bin/env bash

set -eu -o pipefail

# Test if the Dahlia binary is available.
(type dahlia >/dev/null 2>&1 && echo "1. 'dahlia' binary found in the path.") \
  || { echo >&2 "Missing 'dahlia' binary in the path. Exiting."; exit 1; }

(type paralle >/dev/null 2>&1 && echo "1. 'parallel' binary found in the path.") \
  || { echo >&2 "Missing 'parallel' binary in the path. Please install GNU Parallel. Exiting."; exit 1; }

# Test if benchmarking-helpers module has been installed.
(python3 -c "import benchmarking" >/dev/null 2>&1 && echo "2. 'benchmarking' module found for python3.") \
  || { echo >&2 "Cannot find benchmarking helpers library for python3. Please run 'pip3 install -e .' in dahlia-evaluation/benchmarking-helpers"; exit 1; }
