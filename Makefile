all: sample

sample: sample.cpp MIPS_Processor.hpp
	g++ sample.cpp MIPS_Processor.hpp -std=c++11
	./a.out ./Testcases/public_test4.asm


branch_predictor:
	g++ BranchPredictorTesting.cpp BranchPredictor.hpp -std=c++11
	./a.out


clean:
	rm sample