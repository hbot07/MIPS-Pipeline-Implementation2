//
// Created by Parth Thakur on 13/04/23.
//
#include "BranchPredictor.hpp"
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <fstream>
#include <pthread.h>
using namespace std;

std::ifstream file("./Testcases/branchtrace.txt");
std::string line;
std::vector<std::string> lines;
double max_accuracy = 0, max_alpha=0, max_beta=0;
int max_start=0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int n = 12;

void* routine(void* arg) {
    cout << "Thread " + to_string(*(double*)arg) + " started" + to_string(*(double*)arg+1.0/n) + "\n"<< endl;
    for(double alpha=*(double*)arg; alpha<=*(double*)arg+1/n; alpha+=0.001)
        for(double beta=0; beta<=1; beta+=0.001)
            for (int start = 0; start <= 3; start++) {
                //        ofstream bhrFile("./Outputs/SaturatingBHRBranchPredictor_predictions"+ to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) +".txt");
                //        std::cout << "SaturatingBHRBranchPredictor start value: " << to_string(bitset<2>(start)[0]) + to_string(bitset<2>(start)[1]) << std::endl;
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
                    bool predict = bbp.predict(pc_int, alpha, beta);
                    if (predict == taken_bool)
                        correct++;
                    //            bhrFile << pc + " " + to_string(predict) << endl;
                    bbp.update(pc_int, taken_bool);
                }
                //        std::cout << " accuracy: " << (double) correct / (lines.size()) << std::endl; // (lines.size()-1) because the last line is empty
                if((double) correct / (lines.size()) > max_accuracy){
                    pthread_mutex_lock(&mutex1);
                    max_accuracy = (double) correct / (lines.size());
                    max_alpha = alpha;
                    max_beta = beta;
                    max_start = start;
                    pthread_mutex_unlock(&mutex1);
                }
            }
    return nullptr;
}

int main(){
    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    } else {
        std::cerr << "File could not be opened. Terminating...\n";
        return 0;
    }

    //maximise the accuracy of SaturatingBHRBranchPredictor
    pthread_t th[n];
    for(int i=0; i<n; i++){
        double* a = (double*) malloc(sizeof(int));
        *a = ((double)i)/n;
        pthread_create(&th[i], NULL, routine, a);
    }
    for(int i=0; i<n; i++){
        pthread_join(th[i], NULL);
    }

    std::cout << "max accuracy: " << to_string(max_accuracy) << std::endl;
    std::cout << "max alpha: " << to_string(max_alpha) << std::endl;
    std::cout << "max beta: " << to_string(max_beta) << std::endl;
    std::cout << "max start: " << to_string(max_start) << std::endl;
}