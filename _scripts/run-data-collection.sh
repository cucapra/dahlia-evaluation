#!/usr/bin/env bash

set -eu -o pipefail

# List of files that track accepted configurations
BENCH="$1"

# Number of configurations to run for each benchmark.
MAX_CONFS=10

echo "This script reproduces a subset of the data reported in the paper."

echo '================'

# Check if the BUILDBOT server is set.
if [ -z "${BUILDBOT+x}" ]; then
  echo "[Sanity Check] BUILDBOT variable is not set."
  echo "[Sanity Check] Please run 'export BUILDBOT=http://cerberus.cs.cornell.edu:5000' or the hostname of your Polyphemus deployment."
  exit 1;
else
  echo "[Sanity Check] Polyphemus server found: $BUILDBOT."
fi

# Check if accepted configurations have been generated.
for bench in $BENCH; do
  name="$(basename $bench)-accepted"
  if [ ! -f "$name" ]; then
    echo "[Sanity Check] Missing accepted configrations file $name. Did you run '_scripts/run-dahlia-accepts.sh'? The expected file should exist in repository root."
    exit 1
  else
    echo "[Sanity Check] Accepted configurations file $name found."
  fi
done


# For each benchmark, upload MAX_CONFS configurations, wait for results to
# generated, and download the graphs.
for bench in $BENCH; do
  echo '================'
  name="$(basename $bench)-accepted"

  # Generate configrations for the benchmark.
  ./_scripts/gen-dse.py -f $bench

  # Compile fuse files to CPP.
  echo "INFO: Compiling Dahlia files to CPP."
  export FUSE=dahlia
  export MODE=estimate
  cat $name | xargs -n1 dirname | \
    parallel --progress --bar 'cd {} && make fuse' > /dev/null

  # Upload jobs to Polyphemus deployment and the location of the jobs folder.
  job_names=$(cat $name | xargs -n1 dirname | \
    xargs ./_scripts/batch.py -m 'estimate' -b "$(basename $bench)-data")

  echo "INFO: jobs.txt path: $job_names"

  # Remove configuration directory.
  find . -path "./$bench-*" | xargs rm -rf

  # Start status script
  watch -n5 ./_scripts/status.py "$job_names"

done
