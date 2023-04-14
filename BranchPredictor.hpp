#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__

#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>

struct BranchPredictor {
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};

struct SaturatingBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {} // 1<<14 is 2^14

    bool predict(uint32_t pc) {
        // your code here
        int pc_14 = pc & 0x3fff; // 0x3fff is 2^14 - 1
        return table[pc_14][1];
    }

    void update(uint32_t pc, bool taken) {
        // your code here
        int pc_14 = pc & 0x3fff;
        if(taken) {
            if(table[pc_14].operator!=(std::bitset<2>(3)))
                table[pc_14] = std::bitset<2>(table[pc_14].to_ulong() + 1);
        } else {
            if(table[pc_14].operator!=(std::bitset<2>(0)))
                table[pc_14] = std::bitset<2>(table[pc_14].to_ulong() - 1);
        }

    }
};

struct BHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}

    bool predict(uint32_t pc) {
        // your code here
        return bhrTable[bhr.to_ulong()][1];
    }

    void update(uint32_t pc, bool taken) {
        // your code here
        if(taken) {
            if(bhrTable[bhr.to_ulong()].operator!=(std::bitset<2>(3)))
                bhrTable[bhr.to_ulong()] = std::bitset<2>(bhrTable[bhr.to_ulong()].to_ulong() + 1);
        } else {
            if(bhrTable[bhr.to_ulong()].operator!=(std::bitset<2>(0)))
                bhrTable[bhr.to_ulong()] = std::bitset<2>(bhrTable[bhr.to_ulong()].to_ulong() - 1);
        }
        bhr[1] = bhr[0];
        bhr[0] = taken;

    }
};

struct SaturatingBHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value) {
        assert(size <= (1 << 16));
    }

    bool predict(uint32_t pc) {
        // your code here
        int pc_14 = pc & 0x3fff; // 0x3fff is 2^14 - 1
        return (combination[0] & table[pc_14])[1] || (combination[1] & bhrTable[bhr.to_ulong()])[1];
    }

    bool predict(uint32_t pc, double alpha, double beta) {
        // your code here
        int pc_14 = pc & 0x3fff; // 0x3fff is 2^14 - 1
        double guess = table[pc_14][1] * alpha + bhr[1] * (1 - alpha);
        return guess > beta;

    }

    void update(uint32_t pc, bool taken) {
        // your code here
        int pc_14 = pc & 0x3fff;
        if(taken) {
            if(table[pc_14].operator!=(std::bitset<2>(3)))
                table[pc_14] = std::bitset<2>(table[pc_14].to_ulong() + 1);
        } else {
            if(table[pc_14].operator!=(std::bitset<2>(0)))
                table[pc_14] = std::bitset<2>(table[pc_14].to_ulong() - 1);
        }
        if(taken == table[pc_14][1] && combination[1].operator!=(std::bitset<2>(3)))
            combination[0] = std::bitset<2>(combination[0].to_ulong() + 1);

        if(taken) {
            if(bhrTable[bhr.to_ulong()].operator!=(std::bitset<2>(3)))
                bhrTable[bhr.to_ulong()] = std::bitset<2>(bhrTable[bhr.to_ulong()].to_ulong() + 1);
        } else {
            if(bhrTable[bhr.to_ulong()].operator!=(std::bitset<2>(0)))
                bhrTable[bhr.to_ulong()] = std::bitset<2>(bhrTable[bhr.to_ulong()].to_ulong() - 1);
        }
        bhr[1] = bhr[0];
        bhr[0] = taken;
        if(taken == bhrTable[bhr.to_ulong()][1] && combination[1].operator!=(std::bitset<2>(3)))
            combination[1] = std::bitset<2>(combination[1].to_ulong() + 1);
    }
};

#endif