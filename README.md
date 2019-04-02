# Fuse Benchmarks

Benchmarks for the [Fuse programming language](https://github.com/cucapra/seashell).


## Getting Started

Use the top-level Makefile to run all the benchmarks.

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

    ./batch.py <path-to-bench1> <path-to-bench2> ...

### `extract.py`

Downloads results from buildbot given a file with job ids. The job ids file
must contain one job id at each line.

Usage:

    ./extract.py <jobs.txt>


## Directory Structure

For each benchmark, there is directory named `<suite-name>-<bench-name>`.
Each has these subdirectories:

- **original**: This contains the original unedited code from the benchmark
  suite. This is useful comparison and diffing. If we exclude any unnecessary
  data files, we note them in a README file under this directory.
- **baseline**: This a C++ version of the benchmark with the minimal edits
  required for us to run it on our infrastructure.
- **simple-rewrite**: This contains "direct rewrite" of the benchmark in Fuse.
  For the most part, we don't optimize this code at all unless it is trivial to
  make it work with our type system. We use `unroll` when we can.
- **full-rewrite**: This is a full rewrite of the benchmark in Fuse. A full
  rewrite uses features like `views` and `combine` blocks. The code runs the
  same algorithm but looks substantially different from the original.

The `_common` directory contains useful stuff for all benchmarks. The `sds.mk` Makefile snippet therein can be included to make it simple to compile applications with the Xilinx SDSoC toolchain.
