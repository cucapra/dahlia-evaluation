This contains descriptions of SDS pragmas used for benchmarks.

1. [`pragma SDS data zero_copy`](https://www.xilinx.com/html_docs/xilinx2018_2/sdsoc_doc/zero-copy-data-mover-kdq1504034390934.html?hl=data%2Cmover)
This provides a sophisticated and expensive memory controller which can access memory in complex access patterns. This is useful to get simple C algorithms run when arrays are used with hardware unoptimized accesses like reading and writing to and from the same array. For more descriptions check [this](https://github.com/cucapra/epoisses/wiki/Vivado-HLS#4-zero_copyarraynameoffsetlength)
