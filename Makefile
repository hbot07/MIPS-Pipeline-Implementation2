all:
	g++ *.cpp *.hpp -std=c++11
	./a.out ./Testcases/public_test4.asm

branch_predictor:
	g++ BranchPredictorTesting.cpp BranchPredictor.hpp -std=c++11
	./a.out

5stage:
	g++ 5stage.cpp -std=c++11
	./a.out ./Testcases/public_test4.asm

5stage_bypass:
	g++ 5stage_bypass.cpp -std=c++11
	./a.out ./Testcases/public_test4.asm

no_pipeline:
	g++ sample.cpp MIPS_Processor.hpp -std=c++11
	./a.out ./Testcases/public_test4.asm

79stage:
	g++ 79stage.cpp -std=c++11
	./a.out ./Testcases/public_test4.asm

79stage_bypass:
	g++ 79stage_bypass.cpp -std=c++11
	./a.out ./Testcases/public_test4.asm

clean:
	rm a.out
