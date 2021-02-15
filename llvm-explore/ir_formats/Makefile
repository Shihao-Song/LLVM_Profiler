all:
	clang++ main.cc -emit-llvm -c -o main.bc
	llvm-link util/footprint.bc main.bc -o main.linked.bc
	opt -load ~/Documents/PNNL/Profile/llvm-install/lib/LLVMProfiler.so -profiler < main.linked.bc > main.prof.bc
	llvm-dis main.prof.bc -o main.prof.ll
	llc -filetype=obj main.prof.bc -o main.prof.o
	clang++ main.prof.o -o main.prof
