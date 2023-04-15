# MIPS Pipeline Implementation Project

This project is a collection of programs for simulating and analyzing a MIPS Pipeline Processor. The programs include implementations of a 5-stage, 7-stage and 9-stage pipeline processor, as well as versions of those pipelines with bypassing. There is also a program for testing a Branch Predictor, which can be used to improve the performance of the MIPS Pipeline Processor.

The programs are written in C++ and compiled with g++. A Makefile is included for compiling and running the programs.

To compile and run the program, first use the make command to compile the code, then use the corresponding command to run the program with the desired test case:

```
make 5stage
./a.out ./Testcases/public_test4.asm
```

The output will be the results of the simulation. By default, the programs will run on public_test4.asm, which is a test case provided by the course staff. The test cases are located in the Testcases folder. To run another file 
use the following command after using make with the specified target:

```
./a.out ./Testcases/<filename>.asm
```

The following are the available targets:
    
```
make no_pipeline  
make 5stage  
make 5stage_bypass  
make 79stage  
make 79stage_bypass
make branch_predictor
```



The code is organized as follows:

* BranchPredictorTesting.cpp: This program tests the Branch Predictor.
* 5stage.cpp: This program implements a 5-stage MIPS pipeline.
* 5stage_bypass.cpp: This program implements a 5-stage MIPS pipeline with bypassing.
* 7stage.cpp: This program implements a 7-stage MIPS pipeline.
* 7stage_bypass.cpp: This program implements a 7-stage MIPS pipeline with bypassing.
* 9stage.cpp: This program implements a 9-stage MIPS pipeline.
* 9stage_bypass.cpp: This program implements a 9-stage MIPS pipeline with bypassing.
* sample.cpp: This program implements a non-pipelined version of the MIPS processor.
* BranchPredictor.hpp: This file contains the code for the Branch Predictor.
* MIPS_Processor.hpp: This file contains the code for the MIPS Processor.
* Makefile: This file contains the commands for compiling and running the programs.

Thank you for trying out the MIPS Pipeline Implementation Project!