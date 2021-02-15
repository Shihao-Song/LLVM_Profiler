all:
	clang++ footprint.cc -emit-llvm -c -o footprint.raw.bc
	opt -load ~/Documents/PNNL/Profile/llvm-install/lib/LLVMHookFuncsIdentify.so -hook_funcs_identify < footprint.raw.bc > footprint.bc
	llvm-dis footprint.bc -o footprint.ll
	llc -filetype=obj footprint.bc -o footprint.o
	clang++ footprint.o -o footprint
