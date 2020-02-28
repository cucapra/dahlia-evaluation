#!/bin/bash

# Print the status of job IDs from $1. Run on a cerberus server.
set -e

JOBS="$1"
OUT=$(mktemp 'tmp.XXXX')

cat $JOBS | parallel --bar --jobs 200% 'cat jobs/{}/info.json | jq -cr "[.config.hwname, .name, .state] | @csv"' > $OUT

echo 'done'
cat $OUT | grep 'done' | wc -l

echo 'failed'
cat $OUT | grep 'failed' | wc -l

echo 'makeing'
cat $OUT | grep 'makeing' | wc -l

echo 'make'
cat $OUT | grep 'make' | wc -l

echo 'uploaded'
cat $OUT | grep 'uploaded' | wc -l

echo 'unpacking'
cat $OUT | grep 'unpacking' | wc -l

rm $OUT
