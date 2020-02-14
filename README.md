## Dahlia Evaluation

Evaluation for "Predictable Accelerator Design with Time-Sensitive Affine types"
using the [Dahlia programming language](https://github.com/cucapra/dahlia).


### Experiments and Case studies

The paper contains four graphs:

1. Figure 4: Sensitivity analysis of unrolling and partitioning.
2. Figure 7: Exhaustive design space exploration for gemm-blocked.
3. Figure 8: Qualitative study of MachSuite benchmarks.
4. Figure 9: Resource Utilization for gemm-ncubed in [Spatial](https://spatial-lang.org).

The following directories correspond to these experiments.

#### Sensitivity analysis (`sensitivity-analysis/`)

The sensitivity analysis consists of three experiments:

1. Fig. 4a: Unrolling the innermost loop without any partitioning (`sensitivity-analysis/no-partition-unoll`).
2. Fig. 4b: Unrolling with a constant partitioning (`sensitivity-analysis/const-partition-unroll`)
3. Fig. 4c: Unrolling and partitioning in lockstep (`sensitivity-analysis/lockstep-partition-and-unroll`).

#### Exhaustive DSE (`exhaustive-dse/`)

The exhaustive design space exploration study uses a single experiment with
32,000 distinct configurations to generate the three subgraphs in Figure 7.

#### Qualitative study (`qualitative-study/`)

The qualitative study consists of three benchmarks:

1. stencil2d (`qualitative-study/stencil2d`).
2. md-knn (`qualitative-study/md-knn`).
3. md-grid (`qualitative-study/md-grid`).

#### Spatial (`spatial/`)

The Spatial study consists of one experiment with several configurations to
generate Figure 9 (main paper) and Figure 2 (supplementary text).


### Benchmarking Scripts

The infrastructure for running benchmarks is under the `_scripts` directory.

To use the scripts, first install [Requests][] by typing `pip install --user requests`.

For these scripts, you can set a `BUILDBOT` environment variable to point to the URL of the running Buildbot instance.

[requests]: https://2.python-requests.org/en/master/

### `batch.py`

Submit a batch of benchmark jobs to the Buildbot.

Each argument to the script should be the path to a specific benchmark version in this repository, like `baseline/machsuite-gemm-ncubed`.
Use it like this:

    ./_scripts/batch.py <benchpath1> <benchpath2> ...

The script creates a new directory for the batch under `_results/` named with a timestamp.
It puts a list of job IDs in a file called `jobs.txt` there.
It prints the name of the batch directory (i.e., the timestamp) to stdout.

This script has command-line options:

- `-E`: Submit jobs for full synthesis. (The default is to just do estimation.)
- `-p`: Pretend to submit jobs, but don't actually submit anything. (For debugging.)

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

### `status.py`

Get the current status of a batch while you impatiently wait for jobs to complete.
Print out the number of jobs in each state.

Give the script the path to a batch directory:

    ./_scripts/status.py _results/2019-07-13-17-13-09

Use the [watch](https://linux.die.net/man/1/watch) command to repeatedly run
the command every 5 seconds

    watch -n5 ./_scripts/status.py _results/2019-07-13-17-13-09


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

