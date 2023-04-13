#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__

#include <vector>
#include <bitset>
#include <cassert>

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
        return table[pc_14][0];
    }

    void update(uint32_t pc, bool taken) {
        // your code here
        int pc_14 = pc & 0x3fff;
        if(taken) {
            if(table[pc_14].operator==(std::bitset<2>(3)))
                return;
            else
                table[pc_14] = std::bitset<2>(table[pc_14].to_ulong() + 1);
        } else {
            if(table[pc_14].operator==(std::bitset<2>(0)))
                return;
            else
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
        return bhr[0];
    }

    void update(uint32_t pc, bool taken) {
        // your code here
        if(taken) {
            if(bhr.operator==(std::bitset<2>(3)))
                return;
            else
                bhr = std::bitset<2>(bhr.to_ulong() + 1);
        } else {
            if(bhr.operator==(std::bitset<2>(0)))
                return;
            else
                bhr = std::bitset<2>(bhr.to_ulong() - 1);
        }
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
        return false;
    }

    void update(uint32_t pc, bool taken) {
        // your code here
    }
};

#endif