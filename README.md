# Fuse Benchmarks

Benchmarks for the [Fuse programming language](https://github.com/cucapra/seashell).

## Getting started

Use the make file to run all the benchmarks.

- `make all` generates all the C++ files for benchmarks and uploads them to
  [buildbot](http://gorgonzola.cs.cornell.edu:8000/).
- `make extract` will try to download all the jobs that have finished running
  and log any failures in failure-extract.txt.
- `make resume-batch` and `make resume-extract` will re-run
  re-runs the respective scripts with failure logs.

## Benchmarking Scripts

### `batch.py`

This script creates zip files for each benchmark path provided and uploads them
to buildbot and prints out the resulting job ids to STDOUT.

Usage:
```
./batch.py <path-to-bench1> <path-to-bench2> ...
```

### `extract.py`

Downloads results from buildbot given a file with job ids. The job ids file
must contain one job id at each line.

Usage:
```
./extract.py <jobs.txt>
```
