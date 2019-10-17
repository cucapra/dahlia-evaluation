import re

from collections import namedtuple

import extracting

# Names of known kernels
KERNEL_LIST = [
    'aes',
    'backprop',
    'bfs',
    'fft',
    'gemm',
    'kmp',
    'md',
    'nw',
    'sort',
    'spmv',
    'stencil',
    'viterbi'
]

# Instructions for data collection and extraction. In each, `file` is the path
# to a file to download and analyze or a function which either returns a
# filepath or a None when given a filepath. `collect` is a function to call on
# the downloaded file, which should return JSON-serializable information to
# include in the results. `key` is the key to use in the result data.

CollectConfig = namedtuple('CollectConfig', [
    'key',      # Key identifying the collection.
    'file',     # Either a string or a function.
    'collect',  # Function to run on the file to extract data.
])

HLS_RE = re.compile(r'/reports/sds_(\w+).rpt')
SYNTH_RE = re.compile(r'/report/(\w+)_csynth.rpt')
def hls_file(file_path):
    """Find the name of the HLS report, which exists for both Vivado HLS jobs
    and SDSoC jobs---but in different locations!
    """
    # Try to match HLS path regex
    m = HLS_RE.search(file_path)
    if m and m.group(1) != 'main':
        return file_path

    # Try to match SDSOC estimation regex
    m = SYNTH_RE.search(file_path)
    if m:
        if m.group(1) in KERNEL_LIST:
          return file_path
        else:
            logging.warning(
                'Ignoring csynth file because {} is not a known kernel'.format(m.group(1)))

    return None


FULL_SYNTH_RE = re.compile(r'/verilog/report/(\w+)_utilization_synth.rpt')
def full_synth_file(file_path):
    """Find path to the full synthesis report.
    """
    return file_path if FULL_SYNTH_RE.search(file_path) else None

# Collect runtime numbers
COLLECT_RUNTIME = CollectConfig(
    key='runtime',
    file='code/runtime.log',
    collect=extracting.runtime_log,
)

#
COLLECT_UTIL_ROUTED = CollectConfig(
    key='routed_util',
    file='code/_x/reports/link/imp/kernel_util_routed.rpt',
    collect=extracting.util_routed_report,
)

# This one is the output of *estimation*, which runs on the complete
# design (not just a single kernel). It appears to happen *after* HLS.
# It only happens on estimation runs; not on full synthesis runs.
COLLECT_EST = CollectConfig(
    key='est',
    file='code/_sds/est/perf.est',
    collect=extracting.performance_estimates,
)

# Just check for various messages in the job log.
COLLECT_LOG = CollectConfig(
    key='log',
    file='log.txt',
    collect=extracting.log_messages,
)

# This one is the report from HLS-compiling the kernel source code to
# RTL. It seems to exist regardless of whether we're doing estimation. The
# filename here needs to be filled in by searching the file list.
COLLECT_HLS = CollectConfig(
    key='hls',
    file=hls_file,
    collect=extracting.hls_report,
)

# The *overall* report from synthesis. Only available on full runs (not
# estimation). Available in non-estimation SDSoC runs and Vivado HLS runs where
# the tool is configured to run full synthesis, not just HLS itself.
COLLECT_FULL = CollectConfig(
    key='full',
    file=full_synth_file,
    collect=extracting.sds_report,
)

