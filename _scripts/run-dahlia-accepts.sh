#!/usr/bin/env bash

set -eu -o pipefail

BENCH='
  qualitative-study/machsuite-stencil-stencil2d-inner
  qualitative-study/machsuite-md-grid
  qualitative-study/machsuite-md-knn
  exhaustive-dse/dahlia-gemm'

echo "This script will generate configurations for benchmarks in the exhaustive and qualitative studies and report the number of configurations accepted by Dahlia."

echo '================'

if [ ! -z "${ESTIMATE+x}" ]; then
  echo "[Estimate] Running $ESTIMATE jobs for each benchmark."
  echo "[Estimate] This mode reports the total time to run all jobs on a single core machine."
  echo '================'
fi

# Get the current working directory.
dir="$(pwd)"
dse_script="$dir"/_scripts/gen-dse.py
valid_script="$dir"/_scripts/is-valid.sh

echo '[Sanity check] Benchmark directories are present.'
# Make sure benchmark directories are present.
for bench in $BENCH; do
  ([ -d "$dir"/"$bench" ] && echo "[Sanity check] Found $dir/$bench.")  \
    || { echo >&2 "$dir/$bench is not a directory, aborting. Please run this script from the root of the dahlia-evaluations repository"; exit 1; }
done

echo '[Sanity check] All benchmark directories found.'

for bench in $BENCH; do
  echo '================'
  # Generate all configurations
  $dse_script -f "$dir"/"$bench"

  if [ ! -z ${ESTIMATE+x} ]; then
    # Create a time estimate for all the jobs
    start_time=$(date +%s)

    find "$dir" -path "$dir/$bench-*/*.fuse" | \
      head -n $ESTIMATE | \
      parallel -j 1 --progress --bar $valid_script {} > /dev/null || true

    end_time=$(date +%s)
    total_confs=$(find "$dir" -path "$dir/$bench-*/*.fuse" | wc -l)
    echo "[Estimate] Estimated runtime for all jobs on one core: $(((end_time-start_time) * total_confs / 100))s"
  else
    # Create file to record dahlia accepted configurations
    accepted="$(basename $bench)-accepted"
    rm -f "$accepted"

    find "$dir" -path "$dir/$bench-*/*.fuse" | \
      parallel --progress --bar $valid_script {} \
      > "$accepted" || true

    echo "Configurations accepted by Dahlia: $(wc -l $accepted | cut -f1 -d' ')"
  fi

  find "$dir" -path "$dir/$bench-*" | xargs rm -rf

done
