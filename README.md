# Fuse Benchmarks

Benchmarks for the [fuse programming language](https://github.com/cucapra/seashell).


## Getting Started

Use the top-level Makefile to run all the benchmarks.

- `make all` generates all the C++ files for benchmarks and uploads them to
  [buildbot](http://gorgonzola.cs.cornell.edu:8000/).
- `make extract` will try to download all the jobs that have finished running
  and log any failures in failure-extract.txt.
- `make resume-batch` and `make resume-extract` will re-run
  re-runs the respective scripts with failure logs.


## Benchmarking Scripts

The infrastructure for running benchmarks is under the `_scripts` directory.

### `batch.py`

This script creates zip files for each benchmark path provided and uploads them
to buildbot and prints out the resulting job ids to STDOUT. Each argument to the 
script should be the path to a specific benchmark version in this repository, 
for example- `baseline/machsuite-gemm-ncubed`.

Usage:

    ./batch.py <path-to-benchmark-version 1> <path-to-benchmark-version 2> ...

### `extract.py`

Downloads results from buildbot given a file with job ids. The job ids file
must contain one job id at each line.

Usage:

    ./extract.py <jobs.txt>


## Directory Structure

Different designs of benchmarks exist under its own directory. Currently there is,
- **original**: This contains the original unedited code from the [benchmark
  suite.](https://github.com/breagen/MachSuite/) This is useful comparison 
  and diffing. Data files exists and are generated in this directory. Internal 
  folder structure maintains original benchmark suite's structure.
- **baseline**: This contains C++ version of the benchmark with the minimal edits
  required for us to run it on our infrastructure in software and hardware. These changes are documented in
  `docs/` directory under titles SOFTWARE and BASELINE. Runtime information estimators with wall clock and SDSoC provided clock cycle counts are also added. 

To be included are,
- **rewrite**: This will contain one-to-one translation of the benchmark in Fuse.
  For the most part, we don't optimize this code unless it is trivial to
  make it work with our type system. Such changes will be documented in docs.
- **baseline-optimized**: This will contain a manually tuned baseline using 
  HLS hardware optimizations. We won't use major code changes unless it is trivial.
- **fuse-optimized**: This is writing the benchmark from scratch in Fuse. A full
  rewrite will use features like `views` and `combine` blocks. The code runs the
  same algorithm but will look substantially different from the original.

Additionally, there is a **docs** directory to maintain all the changes done for each design and maintain useful information.

Under these directories, there is subdirectory for each benchmark named `<benchmark suite-name>-<benchmark-name>`. Subdirectory `_<benchmark suite name>-templates` contains basic templates used across all the benchmarks. `Makefile` can be used to run all benchmarks locally or in Buildbot without support for failure detection or data extraction.

The `_common` directory contains useful stuff for all designs. The `sds.mk` Makefile snippet therein can be included to make it simple to compile applications with the Xilinx SDSoC toolchain. Subdirectory `_<benchmark suite-name>-common` contains useful stuff for all benchmark versions in that suite. 

