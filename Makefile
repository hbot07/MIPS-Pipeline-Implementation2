all: sample

sample: sample.cpp MIPS_Processor.hpp
	g++ *.cpp *.hpp -std=c++11

clean:
	rm sample