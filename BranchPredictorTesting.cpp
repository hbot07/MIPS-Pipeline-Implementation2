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
    for (int start = 0; start <= 3; start++) {
        ofstream sbpFile("./Outputs/SaturatingBranchPredictor_predictions"+ to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) +".txt");
        std::cout << "SaturatingBranchPredictor initial table values: " << to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) << std::endl;
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
            sbpFile << pc + " " + to_string(predict) << endl;
            sbp.update(pc_int, taken_bool);
    }
    std::cout << " accuracy: " << (double) correct / (lines.size()) << std::endl; // (lines.size()-1) because the last line is empty
    }

    //use BHRBranchPredictor and calculate the accuracy
    for (int start = 0; start <= 3; start++) {
        ofstream bhrFile("./Outputs/BHRBranchPredictor_predictions"+ to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) +".txt");
        std::cout << "BHRBranchPredictor start value: " << to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) << std::endl;
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
            bhrFile << pc + " " + to_string(predict) << endl;
            bbp.update(pc_int, taken_bool);
        }
        std::cout << " accuracy: " << (double) correct / (lines.size()) << std::endl; // (lines.size()-1) because the last line is empty
    }

    //use SaturatingBHRBranchPredictor and calculate the accuracy
    for (int start = 0; start <= 3; start++) {
        ofstream bhrFile("./Outputs/SaturatingBHRBranchPredictor_predictions"+ to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) +".txt");
        std::cout << "SaturatingBHRBranchPredictor start value: " << to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) << std::endl;
        SaturatingBHRBranchPredictor bbp(start, 2);
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
            bhrFile << pc + " " + to_string(predict) << endl;
            bbp.update(pc_int, taken_bool);
        }
        std::cout << " accuracy: " << (double) correct / (lines.size()) << std::endl; // (lines.size()-1) because the last line is empty
    }

//    //maximise the accuracy of SaturatingBHRBranchPredictor
//    double max_accuracy = 0, max_alpha=0, max_beta=0;
//    int max_start=0;
//    for(double alpha=0; alpha<=1; alpha+=0.0001)
//        for(double beta=0; beta<=1; beta+=0.01)
//        for (int start = 0; start <= 3; start++) {
//    //        ofstream bhrFile("./Outputs/SaturatingBHRBranchPredictor_predictions"+ to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) +".txt");
//    //        std::cout << "SaturatingBHRBranchPredictor start value: " << to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) << std::endl;
//            SaturatingBHRBranchPredictor bbp(start, 2);
//            long correct = 0;
//            for (int i = 0; i < lines.size(); i++) {
//                std::string line = lines[i];
//                std::string pc = line.substr(0, 8);
//                std::string taken = line.substr(9, 1);
//                uint32_t pc_int = std::stol(pc, nullptr, 16);
//                bool taken_bool;
//                if (taken == "1") {
//                    taken_bool = true;
//                } else {
//                    if (taken == "0")
//                        taken_bool = false;
//                }
//                bool predict = bbp.predict(pc_int, alpha, beta);
//                if (predict == taken_bool)
//                    correct++;
//    //            bhrFile << pc + " " + to_string(predict) << endl;
//                bbp.update(pc_int, taken_bool);
//            }
//    //        std::cout << " accuracy: " << (double) correct / (lines.size()) << std::endl; // (lines.size()-1) because the last line is empty
//            if((double) correct / (lines.size()) > max_accuracy){
//                max_accuracy = (double) correct / (lines.size());
//                max_alpha = alpha;
//                max_beta = beta;
//                max_start = start;
//            }
//        }
//    std::cout << "max accuracy: " << to_string(max_accuracy) << std::endl;
//    std::cout << "max alpha: " << to_string(max_alpha) << std::endl;
//    std::cout << "max beta: " << to_string(max_beta) << std::endl;
//    std::cout << "max start: " << to_string(max_start) << std::endl;
}