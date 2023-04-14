/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __MIPS_PROCESSOR_HPP__
#define __MIPS_PROCESSOR_HPP__

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>

struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	int reg_flags[32] = {0};
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};

		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		for(int i = 0;i < 32;i++)
		    reg_flags[i] = 0;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

     std::unordered_map<std::string, int> instructions2 =
                {{"add", 0}, {"sub", 1}, {"mul", 2}, {"beq", 3}, {"bne", 4}, {"slt", 5}, {"j", 6}, {"lw", 7}, {"sw", 8}, {"addi", 9}, {"sll", 10}, {"srl", 11},{"last",-1}};

    
    int instructionDecode(std::vector<std::string> &command) {
       
        return instructions2[command[0]];
    }

    int aluExecute( int rsValue,  int rtValue,  int immediate,  int function) {
        int result = 0;
        // Perform the appropriate ALU operation based on the function code
        switch (function) {
            case 0: // add
                result = rsValue + rtValue;
                break;
            case 1: // sub
                result = rsValue - rtValue;
                break;
            case 2: // mul
                result = rsValue * rtValue;
                break;
            case 5: // slt
                result = rsValue < rtValue ? 1:0;
                break;
            case 9:  // addi
                result = rtValue + immediate;
                break;
            case 3: //beq
                result = rsValue == rtValue ? 1 : 0;
                break;
            case 4: //beq
                result = rsValue != rtValue ? 1 : 0;
                break;
            case 7: // lw
                result = rtValue + immediate;
                break;
            case 8: // sw
                result = rtValue + immediate;
                break;
            case 10:
                result = rtValue << immediate;
                break;
             case 11:
                result = rtValue >> immediate;
                break;    
            default:
                result = immediate; // patani kab kaam aajae
                break;
        }
        return result;
    }

  void writeBack(unsigned int memData, unsigned int rd, unsigned int* reg) {
        // Write the value of memData to the register specified by rd
        reg[rd] = memData;
    }


void write_mem(int datain, int address){

data[address] = datain;

}

int read_mem(int address){
return data[address];
}




	// perform add operation
	int add(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a + b; });
	}

	// perform subtraction operation
	int sub(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a - b; });
	}

	// perform multiplication operation
	int mul(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a * b; });
	}

	// perform the binary operation
	int op(std::string r1, std::string r2, std::string r3, std::function<int(int, int)> operation)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the beq operation
	int beq(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a == b; });
	}

	// perform the bne operation
	int bne(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a != b; });
	}

	// implements beq and bne by taking the comparator
	int bOP(std::string r1, std::string r2, std::string label, std::function<bool(int, int)> comp)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		if (!checkRegisters({r1, r2}))
			return 1;
		PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCcurr + 1;
		return 0;
	}

	// implements slt operation
	int slt(std::string r1, std::string r2, std::string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] < registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the jump operation
	int j(std::string label, std::string unused1 = "", std::string unused2 = "")
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r) || registerMap[r] == 0)
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		registers[registerMap[r]] = data[address];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform store word operation
	int sw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		data[address] = registers[registerMap[r]];
		PCnext = PCcurr + 1;
		return 0;
	}

	int locateAddress(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				int address = registers[registerMap[reg]] + offset;
				if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
					return -3;
				return address / 4;
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address % 4 || address < int(4 * commands.size()) || address >= MAX)
				return -3;
			return address / 4;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}

	// perform add immediate operation
	int addi(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}

	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}

	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}

	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		std::cout << '\n';
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
		std::cout << "\nFollowing are the non-zero data values:\n";
		for (int i = 0; i < MAX / 4; ++i)
			if (data[i] != 0)
				std::cout << 4 * i << '-' << 4 * i + 3 << std::hex << ": " << data[i] << '\n'
						  << std::dec;
		std::cout << "\nTotal number of cycles: " << cycleCount << '\n';
		std::cout << "Count of instructions executed:\n";
		for (int i = 0; i < (int)commands.size(); ++i)
		{
			std::cout << commandCount[i] << " times:\t";
			for (auto &s : commands[i])
				std::cout << s << ' ';
			std::cout << '\n';
		}
	}

	// parse the command assuming correctly formatted MIPS instruction (or label)
	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t()"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);
	}

	// construct the commands vector from the input file
	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}

	// execute the commands sequentially (no pipelining)
	void executeCommandsUnpipelined()
	{
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}
		printRegisters(0);
                PCcurr = 1;
		int clockCycles = 1;
		std::vector<std::string> &command = commands[0];
		std::vector<std::string> &commandbuffer = commands[0];
		int  if_flag = 1;
		std::string command0,command1,command2,command3,mem_reg_buffer,mem_reg,mem_op_buffer,mem_op;
		std::string command0buffer,command1buffer,command2buffer,command3buffer;
		int alu_result,alu_result_buffer,memory_out_buffer,memory_out;
		alu_result = 0;
		alu_result_buffer = 0;
		memory_out = 0;
		memory_out_buffer = 0;
		int branch_flag = 0;
		int branch_flag1 = 0;
		int wb_flag = 1;
		int dec_flag = 1;
		int mem_flag = 1;
		int alu_flag = 1;
		int branch_end = 0;
		printRegisters(clockCycles);
		++commandCount[0];
		while (1)
		{
		
			
		start:	
			
			// wb stage
			mem_flag = 1;
			switch (instructions2[mem_op]){
			case 7:
			      registers[registerMap[mem_reg]] = memory_out;
		              if (reg_flags[registerMap[mem_reg]]){reg_flags[registerMap[mem_reg]] = 0;}
		              break;
		        
		        case 3:
		        case 4:
		        case 6:
		        case 8:
		              break;
		        
			default:
			      registers[registerMap[mem_reg]] = alu_result_buffer;
			     
                             if (reg_flags[registerMap[mem_reg]]){reg_flags[registerMap[mem_reg]] = 0;}
                             break; 
			}
			if (instructions2[mem_op] == -1 ){ break;}
			if(/*(instructions2[command[0]] == -1 &&(instructions2[mem_op] == 4 || instructions2[mem_op] == 3))&&*/branch_end){
			clockCycles++;
			printRegisters(clockCycles);
			break;
			}
			std::cout<< mem_op<<" done\n";
			
			
			//mem stage
			
			if (mem_flag){
			mem_op = mem_op_buffer;
			mem_reg = mem_reg_buffer;
                       alu_result_buffer = alu_result;

                       //alu_flag = 1;			
			switch(instructions2[mem_op_buffer]){
			case 8:
			     data[alu_result/4] = registers[registerMap[mem_reg_buffer]];

			  
			     break;
			case 7:
			      memory_out = data[alu_result/4];
			      break;
			default:
			      memory_out = memory_out;
			      break;
			}                             
			std::cout<< mem_op_buffer<<" mem\n";                    // requires mem_reg and and mem_op
			}else{alu_flag = 0;}
			
			
			//alu stage
			if (branch_flag == 1){
			branch_flag = 0;
			goto alu_skip;
			}
			else{if(branch_flag == 2){branch_flag = 1; if_flag = 1;goto dec_skip;}}
			if(alu_flag){
			mem_op_buffer = command0;
			mem_reg_buffer = command1;
			                                                  //requires comman parameters of previous instruction
			switch(instructions2[command0]){
			case 7:
			case 8:
			case 10:
			case 11:
			case 9:
			    alu_result = aluExecute(0,registers[registerMap[command2]],stoi(command3),instructions2[command0]);
			    break;
			 
		        case 3:
		        case 4:
		            branch_flag1 = aluExecute(registers[registerMap[command1]],registers[registerMap[command2]],0,instructions2[command0]);
		            clockCycles++;
                           printRegisters(clockCycles);
                           branch_flag = 2;
                           if (branch_flag1){PCcurr = address[command3];}
                            goto start;
		            
		            std::cout<< command3 << "\n";
			case -1:
			    break;
			default:
			    alu_result = aluExecute(registers[registerMap[command2]],registers[registerMap[command3]],0,instructions2[command0]);
			    break;
			}
			dec_flag = 1;
                       std::cout<< command0  <<" alu\n";
                     
                       
                        }
                        
                        
                      
                      alu_skip:
                      
                      //dec stage
                      
                      if(dec_flag){
                      
                      
                      
                      
                     /* if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(SYNTAX_ERROR, clockCycles);
				return;
			}*/ 
			
			
                        if_flag = 1;
                
                        switch(instructions2[command[0]]){
            //addi,srl and  sll
                        case 9:
                        case 10:
                        case 11:
                        if (!(reg_flags[registerMap[command[2]]])){
                        command0 = command[0];
                        command3 = command[3];
                        command2 = command[2];
                        command1 = command[1];
                        reg_flags[registerMap[command[1]]] = 1;
                        alu_flag = 1;
                        }
                        else{alu_flag = 0;if_flag = 0;}
                        break;
                        
                        case 6:
                              std::cout << command[1] << "\n";
                              clockCycles++;
                              printRegisters(clockCycles);
                              branch_flag = 2;PCcurr = address[command[1]];
                        //      if(PCcurr == commands.size()){clockCycles++;printRegisters(clockCycles);break;}
                              
                              goto start;
                             
                        //memory_ops
                       case 7:
                       if (!(reg_flags[registerMap[command[3]]])){  command0 = command[0];
                        command3 = command[2];
                        command2 = command[3];
                        command1 = command[1];
                      
                        alu_flag = 1;
                        reg_flags[registerMap[command[1]]] = 1;}else{alu_flag = 0;if_flag = 0;}
                        break;
                       case 8:
                        if (!(reg_flags[registerMap[command[3]]]) && !(reg_flags[registerMap[command[1]]])){
                        command0 = command[0];
                        command3 = command[2];
                        command2 = command[3];
                        command1 = command[1];
                      
                        alu_flag = 1;
                        }
                        else{alu_flag = 0;if_flag = 0;}
                        break;
                      /*  case 9:
                        if (!(reg_flags[registerMap[command[2]]])){
                        command0 = command[0];
                        command3 = command[3];
                        command2 = command[2];
                        command1 = command[1];
                        }
                        else{if_flag = 0;}
                        break;*/
                        
                       //r_type
                       case -1:
                         command0 = command[0];
                         alu_flag = 1;
                         if_flag = 0;
                         break;
                        case 3:
                        case 4:
                        if (!(reg_flags[registerMap[command[1]]])&& !(reg_flags[registerMap[command[2]]])){
                        command0 = command[0];
                        command3 = command[3];
                        command2 = command[2];
                        command1 = command[1];
                        alu_flag = 1;
                        if_flag = 0;
                        }
                        else{alu_flag = 0;if_flag = 0;}
                        break;
              
                        default:
                        if (!(reg_flags[registerMap[command[2]]])&& !(reg_flags[registerMap[command[3]]])){
                        command0 = command[0];
                        command3 = command[3];
                        command2 = command[2];
                        command1 = command[1];
                        reg_flags[registerMap[command[1]]] = 1;
                        alu_flag = 1;
                        }
                        else{alu_flag = 0;if_flag = 0;}
                        break;
                        
                        }
                       /* switch(instructions2[command[0]]){
                        case 8:
                        break;
                        case 3:
                        case 4:
                        case -1:
                         break;
              
                        default:
                        break;  
                        }*/
                        
                        std::cout << command[0] << " decode\n";
                        }
                        else{if_flag = 0;}
                           
                      dec_skip:     
                           
                      //if stage
			if (if_flag){
			if (PCcurr < (commands.size())){
			command = commands[PCcurr];
			}
			else{command[0] = "last";if_flag = 0;
			if(branch_flag == 1)
			  branch_end = 1;
			}
			++commandCount[PCcurr];
			PCnext = PCcurr + 1;
			PCcurr = PCnext;
			
			std::cout << command[0] << " if\n";
	}
			
			++clockCycles;
			printRegisters(clockCycles);			
	}		
		handleExit(SUCCESS, clockCycles);
	}

	// print the register data in hexadecimal
	void printRegisters(int clockCycle)
	{
		std::cout << "Cycle number: " << clockCycle << '\n'
				  << std::hex;
		for (int i = 0; i < 32; ++i)
			std::cout << registers[i] << ' ';
		std::cout << std::dec << '\n';
	}

};

#endif
