all: sample

sample: sample.cpp MIPS_Processor.hpp
	g++ sample.cpp MIPS_Processor.hpp -std=c++11

clean:
	rm sample