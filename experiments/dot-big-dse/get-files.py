#!/usr/bin/env python3

from benchmarking import common
from benchmarking.summary import extracting, collect_configs
from benchmarking.polyphemus.extract import extract_batch

OUT_DIR = 'data'

if __name__ == '__main__':

    static_est = collect_configs.CollectConfig(
        key='sda_est',
        file='code/_x/reports/dot.sw_emu.xilinx_aws-vu9p-f1-04261818_dynamic_5_0/system_estimate_dot.sw_emu.xilinx_aws-vu9p-f1-04261818_dynamic_5_0.xtxt',
        collect=extracting.sdaccel_estimate
    )

    common.logging_setup()
    static_collects = [ static_est, collect_configs.VALID_COLLECTS['dse_template'] ]
    extract_batch(OUT_DIR, static_collects, [])
