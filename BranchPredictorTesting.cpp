//
// Created by Parth Thakur on 13/04/23.
//

#include "BranchPredictor.hpp"
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char *argv[]) {
    std::ifstream file("./Testcases/branchtrace.txt");
    std::string line;
    std::vector<std::string> lines;
    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "File could not be opened. Terminating...\n";
        return 0;
    }

    //use SaturatingBranchPredictor and calculate the accuracy
    ofstream MyFile("./Outputs/SaturatingBranchPredictor_predictions.txt");
    for (int start = 0; start <= 3; start++) {
        std::cout << "SaturatingBranchPredictor initial table values: " << start << std::endl;
        SaturatingBranchPredictor sbp(start);
        long correct = 0;
        for (int i = 0; i < lines.size(); i++) {
            std::string line = lines[i];
            std::string pc = line.substr(0, 8);
            std::string taken = line.substr(9, 1);
            uint32_t pc_int = std::stol(pc, nullptr, 16);
            bool taken_bool;
            if (taken == "1") {
                taken_bool = true;
            } else {
                if (taken == "0")
                    taken_bool = false;
            }
            bool predict = sbp.predict(pc_int);
            if (predict == taken_bool)
                correct++;
            sbp.update(pc_int, taken_bool);
    }
    std::cout << " accuracy: " << (double) correct / lines.size() << std::endl;
    }
    //use BHRBranchPredictor and calculate the accuracy
    for (int start = 0; start <= 3; start++) {
        std::cout << "BHRBranchPredictor start value: " << start << std::endl;
        BHRBranchPredictor bbp(start);
        long correct = 0;
        for (int i = 0; i < lines.size(); i++) {
            std::string line = lines[i];
            std::string pc = line.substr(0, 8);
            std::string taken = line.substr(9, 1);
            uint32_t pc_int = std::stol(pc, nullptr, 16);
            bool taken_bool;
            if (taken == "1") {
                taken_bool = true;
            } else {
                if (taken == "0")
                    taken_bool = false;
            }
            bool predict = bbp.predict(pc_int);
            if (predict == taken_bool)
                correct++;
            bbp.update(pc_int, taken_bool);
        }
        std::cout << " accuracy: " << (double) correct / lines.size() << std::endl;
    }
}