This contains descriptions of SDS pragmas used for benchmarks.

1. [`pragma SDS data zero_copy`](https://www.xilinx.com/html_docs/xilinx2018_2/sdsoc_doc/zero-copy-data-mover-kdq1504034390934.html?hl=data%2Cmover)
This provides a sophisticated and expensive memory controller which can access memory in complex access patterns. This is useful to get simple C algorithms run when arrays are used with hardware unoptimized accesses like reading and writing to and from the same array. For more descriptions check [this](https://github.com/cucapra/epoisses/wiki/Vivado-HLS#4-zero_copyarraynameoffsetlength)

2. [`pragma SDS data copy`](https://www.xilinx.com/html_docs/xilinx2018_2/sdsoc_doc/vff1524546986267.html?hl=data%2Ccopy)
This can be used to specify the data size of a function argument. This is useful to explicitly provide this information to SDSoC when it's struggling to infer the size automatically (check [bug report 2](https://github.com/cucapra/fuse-benchmarks/blob/master/docs/SDSoC/sds_bug_reports.md#bug-report-2) for an example). For more descriptions check [this](https://github.com/cucapra/epoisses/wiki/Vivado-HLS#3-copyarraynameoffsetlength)
