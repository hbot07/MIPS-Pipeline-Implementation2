all: sample

sample: sample.cpp MIPS_Processor.hpp
	g++ *.cpp *.hpp BranchPredictor.hpp -std=c++11

branch_predictor:
	g++ BranchPredictorTesting.cpp BranchPredictor.hpp -std=c++11
	./a.out


clean:
	rm sample