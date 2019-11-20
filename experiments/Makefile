EXPS := gemm-no-part-unroll gemm-const-unroll-part gemm-const-partition-unroll gemm-const-len-partition-and-unroll gemm-blocked-dse

paper-experiments:
	for exp in $(EXPS); do $(MAKE) -C $$exp -f extract.mk; done
