from collections import namedtuple

SummarizeConfig = namedtuple('SummarizeConfig', [
    'name', # Name of the configuration
    'keys', # Keys and paths to extract them from.
])

# Specification for commonly extracted keys. A key of the job a:b:c accesses
# the field job_results[a][b][c].

SUMMARIZE_CONFIGS = [
    SummarizeConfig(
        name='hls',
        keys={
            'hls_lat_min':  'results:hls:min_latency' ,
            'hls_lat_max':  'results:hls:max_latency',
            'hls_lut':      'results:hls:lut_used',
            'hls_ff':       'results:hls:ff_used',
            'hls_bram':     'results:hls:bram_used',
            'hls_dsp':      'results:hls:dsp48_used',
        }
    ),

    SummarizeConfig(
        name='sds_est',
        keys={
            'est_lat':  'results:est:hw_latency' ,
            'est_lut':  'results:est:lut_used',
            'est_ff':   'results:est:ff_used',
            'est_bram': 'results:est:bram_used',
            'est_dsp':  'results:est:dsp_used',
        }
    ),

    SummarizeConfig(
        name='sda_est',
        keys={
            'est_lut':  'results:sda_est:LUT',
            'est_ff':   'results:sda_est:FF',
            'est_bram': 'results:sda_est:BRAM',
            'est_dsp':  'results:sda_est:DSP',
        }
    ),

    SummarizeConfig(
        name='sds_est',
        keys={
            'full_lut':     'results:full:lut_used',
            'full_ff':      'results:full:reg_ff_used',
            'full_bram':    'results:full:dsp_used',
            'full_dsp':     'results:full:dsp_used',
        }
    ),

    SummarizeConfig(
        name='sda_conf',
        keys={
            'target_freq': 'job:config:make_conf:TARGET_FREQ'
        }
    ),

    SummarizeConfig(
        name='routed_util',
        keys={
            "bram_used":    "results:routed_util:bram_used",
            "dsp_used":     "results:routed_util:dsp_used",
            "lut_used":     "results:routed_util:lut_used",
            "lutmem_used":  "results:routed_util:lutmem_used",
            "reg_used":     "results:routed_util:reg_used",
            "uram_used":    "results:routed_util:uram_used",
        }
    ),

    SummarizeConfig(
        name='default',
        keys={
            'bench': 'bench',
        }
    ),
]

CONF_MAP = { conf.name: conf for conf in SUMMARIZE_CONFIGS }

def parse_confs(conf):
    assert conf in CONF_MAP.keys(), "Unknown configuration {}. Must be one of: {}".format(conf, ', '.join(CONF_MAP.keys()))
    return CONF_MAP[conf]
