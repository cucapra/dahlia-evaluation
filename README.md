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

There is also a `make fuse` target to recompile all the source code to HLS C++.


## Benchmarking Scripts

The infrastructure for running benchmarks is under the `_scripts` directory.
For these scripts, you can set a `BUILDBOT` environment variable to point to the URL of the running Buildbot instance.

### `batch.py`

Submit a batch of benchmark jobs to the Buildbot.

Each argument to the script should be the path to a specific benchmark version in this repository, like `baseline/machsuite-gemm-ncubed`.
Use it like this:

    ./_scripts/batch.py <benchpath1> <benchpath2> ...

The script creates a new directory for the batch under `_results/` named with a timestamp.
It puts a list of job IDs in a file called `jobs.txt` there.
It prints the name of the batch directory (i.e., the timestamp) to stdout.

### `extract.py`

Download results for a previously-submitted batch of benchmark jobs.

On the command line, give the path to the batch directory.
Like this:

    ./_scripts/extract.py _results/2019-07-13-17-13-09

The script downloads information about jobs from `jobs.txt` in that directory.
It saves lots of extracted result values for the batch in a file called `results.json` there.

### `summarize.py`

Given some extracted data for a batch, summarize the results in a human-friendly CSV.

Give the script the path to a `results.json`, like this:

    ./_scripts/summarize.py _results/2019-07-13-17-13-09/results.json

The script produces a file in the same directory called `summary.csv` with particularly relevant information pulled out.


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

