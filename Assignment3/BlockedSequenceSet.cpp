#include "BlockedSequenceSet.h"
#include "Block.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstdint>
using namespace std;

BlockedSequenceSet::BlockedSequenceSet(const std::string& fname) : filename(fname) {
    blocks.clear();
}

void BlockedSequenceSet::AddRecord(const std::string& rec) {
    if (blocks.empty() || blocks.back().GetFreeSpace() < static_cast<int>(rec.size())) {
        Block newBlock(blocks.size(), 512);
        blocks.push_back(newBlock);
    }
    blocks.back().AddRecord(rec);
}

void BlockedSequenceSet::WriteToFile() {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }
    for (const auto& block : blocks) block.Write(out);
    out.close();
}

void BlockedSequenceSet::PrintSummary() const {
    std::cout << "BlockedSequenceSet Summary:\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Total records: " << GetTotalRecords() << "\n";
    std::cout << "Total blocks: " << GetTotalBlocks() << "\n";
    for (const auto& block : blocks) block.PrintSummary();
}

int BlockedSequenceSet::GetTotalRecords() const {
    int total = 0;
    for (const auto& block : blocks) total += block.GetRecordCount();
    return total;
}

int BlockedSequenceSet::GetTotalBlocks() const {
    return blocks.size();
}

bool BlockedSequenceSet::Search(const std::string& key, std::string& outRecord)
{
    for (const Block& block : blocks)
    {
        for (const std::string& rec : block.getRecords())
        {
            // Each record is CSV; extract the key (Zip Code)
            std::string recordKey = rec.substr(0, rec.find(','));

            if (recordKey == key)
            {
                outRecord = rec;
                return true;
            }
        }
    }
    return false;
}


void BlockedSequenceSet::Insert(const std::string& record)
{
    std::string key = record.substr(0, record.find(','));

    for (Block& block : blocks)
    {
        if (block.GetRecordCount() == 0 ||
            key <= block.getHighestKey())
        {
            block.InsertSorted(record);
            return;
        }
    }

    // if no block found, append to last block
    if (blocks.empty() || !blocks.back().HasSpace(record))
    {
        blocks.push_back(Block(blocks.size(), 512));
    }

    blocks.back().InsertSorted(record);
}

bool BlockedSequenceSet::Delete(const std::string& key)
{
    for (Block& block : blocks)
    {
        if (block.DeleteRecord(key))
        {
            return true;
        }
    }
    return false;
}

const std::vector<Block> BlockedSequenceSet::getBlocks() const {
    return blocks;
}

const std::vector<std::string> BlockedSequenceSet::getRecords() const 
{
    std::vector<std::string> allRecords;
    for (const auto& block : blocks) {
        const auto& recs = block.getRecords();
        allRecords.insert(allRecords.end(), recs.begin(), recs.end());
    }
    return allRecords;
}

void BlockedSequenceSet::dumpPhysicalOrder()
{ 
    for(const Block& block: blocks) 
    {
        block.DumpContents();
        
    }
}

void BlockedSequenceSet::dumpLogicOrder()
{
    if (blocks.empty()) return;

    // Build a map from RBN to Block pointer for quick lookup
    std::map<int, const Block*> rbnToBlock;
    for (const auto& block : blocks) {
        rbnToBlock[block.GetRBN()] = &block;
    }

    // Find the logical head block (the one with no predecessor)
    int headRBN = -1;
    for (const auto& block : blocks) {
        if (block.GetPrevRBN() == -1) {
            headRBN = block.GetRBN();
            break;
        }
    }
    if (headRBN == -1) return; // No head found

    // Traverse blocks in logical order
    int currentRBN = headRBN;
    while (rbnToBlock.count(currentRBN)) {
        const Block* block = rbnToBlock[currentRBN];
        block->DumpLogicOrder();
        int nextRBN = block->GetNextRBN();
        if (nextRBN == -1 || nextRBN == currentRBN) break;
        currentRBN = nextRBN;
    }
}