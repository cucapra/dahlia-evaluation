## Dahlia Evaluation

Evaluation for "Predictable Accelerator Design with Time-Sensitive Affine types"
using the [Dahlia programming language](https://github.com/cucapra/dahlia).

### Data Collection

Each experiment goes through the following flow:

<p align="center">
  <img src="./static/data-collection.svg">
</p>

<details>
<summary><b>Example Configuration</b> [click to expand]</summary>

`gen_dse.py` is a search and replace script that generates folders for each
possible configuration.

When invoked on a folder, it looks for a `template.json` file that maps
paramters in files to possible values. For example, the following in
files in a folder named `bench`:

<table>
<tr> <th> bench.cpp </th> <th> template.json </th> </tr>
<tr>
<td>
<pre>
int x = ::CONST1::;
int y = ::CONST2::;
x + y;
</pre>
</td>
<td>
<pre>
{
  "bench.cpp": {
    "CONST1": [1, 2, 3],
    "CONST2": [1, 2, 3]
  }
}
</pre>
</td>
</tr>
</table>

`gen_dse.py` will generate 9 configurations in total by iterating over the
possible values of `CONST1` and `CONST2`.
</details>

### Experiments and Case studies

The paper contains four graphs:

1. Figure 4: Sensitivity analysis of unrolling and partitioning.
2. Figure 7: Exhaustive design space exploration for gemm-blocked.
3. Figure 8: Qualitative study of MachSuite benchmarks.
4. Figure 9: Resource Utilization for gemm-ncubed in [Spatial](https://spatial-lang.org).

The following directories correspond to these experiments.

**Sensitivity analysis** (`sensitivity-analysis/`)

The sensitivity analysis consists of three experiments:

1. Fig. 4a: Unrolling the innermost loop without any partitioning (`sensitivity-analysis/no-partition-unoll`).
2. Fig. 4b: Unrolling with a constant partitioning (`sensitivity-analysis/const-partition-unroll`)
3. Fig. 4c: Unrolling and partitioning in lockstep (`sensitivity-analysis/lockstep-partition-and-unroll`).

**Exhaustive DSE** (`exhaustive-dse/`)

The exhaustive design space exploration study uses a single experiment with
32,000 distinct configurations to generate the three subgraphs in Figure 7.

**Qualitative study** (`qualitative-study/`)

The qualitative study consists of three benchmarks:

1. stencil2d (`qualitative-study/stencil2d`).
2. md-knn (`qualitative-study/md-knn`).
3. md-grid (`qualitative-study/md-grid`).

**Spatial** (`spatial/`)

The Spatial study consists of one experiment with several configurations to
generate Figure 9 (main paper) and Figure 2 (supplementary text).

----------------


### Benchmarking Scripts

The infrastructure for running benchmarks is under the `_scripts` directory.

To use the scripts, first install [Requests][] by typing `pip install --user
requests`.

For these scripts, you can set a `BUILDBOT` environment variable to point to
the URL of the running Buildbot instance.

[requests]: https://2.python-requests.org/en/master/

<details>
<summary><b>batch.py</b> [click to expand]</summary>
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
</details>

<details>
<summary><b>extract.py</b> [click to expand]</summary>
Download results for a previously-submitted batch of benchmark jobs.

On the command line, give the path to the batch directory.
Like this:

    ./_scripts/extract.py _results/2019-07-13-17-13-09

The script downloads information about jobs from `jobs.txt` in that directory.
It saves lots of extracted result values for the batch in a file called `results.json` there.

</details>

<details>
<summary><b>summarize.py</b> [click to expand]</summary>
Given some extracted data for a batch, summarize the results in a human-friendly CSV.

Give the script the path to a `results.json`, like this:

    ./_scripts/summarize.py _results/2019-07-13-17-13-09/results.json

The script produces a file in the same directory called `summary.csv` with particularly relevant information pulled out.
</details>

<details>
<summary><b>status.py</b> [click to expand]</summary>
Get the current status of a batch while you impatiently wait for jobs to complete.
Print out the number of jobs in each state.

Give the script the path to a batch directory:

    ./_scripts/status.py _results/2019-07-13-17-13-09

Use the [watch](https://linux.die.net/man/1/watch) command to repeatedly run
the command every 5 seconds

    watch -n5 ./_scripts/status.py _results/2019-07-13-17-13-09
</details>
