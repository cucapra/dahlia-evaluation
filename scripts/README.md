Benchmarking Scripts
---------------

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
