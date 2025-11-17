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
    bool Search(const std::string& key, std::string& outRecord);
    void Insert(const std::string& record);
    bool Delete(const std::string& key);

};

#endif
