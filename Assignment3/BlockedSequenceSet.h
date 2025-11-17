#ifndef BLOCKEDSEQUENCESET_H
#define BLOCKEDSEQUENCESET_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Block.h"

class BlockedSequenceSet {
private:
    std::vector<Block> blocks;
    std::string filename;

public:
    BlockedSequenceSet(const std::string& fname);
    void AddRecord(const std::string& rec);
    void WriteToFile();
    void PrintSummary() const;
    int GetTotalRecords() const;
    int GetTotalBlocks() const;
    const std::vector<Block> getBlocks() const;
    const std::vector<std::string> getRecords() const;
    void dumpPhysicalOrder();
    void dumpLogicOrder();

};

#endif
