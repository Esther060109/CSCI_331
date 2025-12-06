/**
 * @file BlockedSequenceSet.cpp
 * @brief Implements the BlockedSequenceSet class, which manages a collection
 *        of fixed-size blocks storing variable-length string records.
 *
 * This file defines all core operations for adding, searching, inserting,
 * deleting, serializing, and displaying records across multiple blocks.
 */
#include "BlockedSequenceSet.h"
#include "Block.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <cstdint>
using namespace std;

/**
 * @brief Constructs a BlockedSequenceSet associated with a target filename.
 *
 * @param fname Name of the file to which blocks will eventually be written.
 *
 * Initializes an empty block vector.
 */
BlockedSequenceSet::BlockedSequenceSet(const std::string& fname) : filename(fname) {
    blocks.clear();
}

/**
 * @brief Adds a record to the last block or creates a new block if necessary.
 *
 * @param rec The record string to be added.
 *
 * If the last block does not have enough space for the record, a new block
 * is created. Each new block has a default size of 512 bytes.
 */
void BlockedSequenceSet::AddRecord(const std::string& rec) {
    if (blocks.empty() || blocks.back().GetFreeSpace() < static_cast<int>(rec.size())) {
        Block newBlock(blocks.size(), 512);
        blocks.push_back(newBlock);
    }
    blocks.back().AddRecord(rec);
}

/**
 * @brief Serializes all blocks to the configured file in human-readable format.
 *
 * Each block is written using Block::Write().
 * Existing file content is overwritten.
 */
void BlockedSequenceSet::WriteToFile() {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }
    for (const auto& block : blocks) block.Write(out);
    out.close();
}

/**
 * @brief Prints a summary of the BlockedSequenceSet.
 *
 * Includes total records, total blocks, filename, and per-block summaries.
 */
void BlockedSequenceSet::PrintSummary() const {
    std::cout << "BlockedSequenceSet Summary:\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Total records: " << GetTotalRecords() << "\n";
    std::cout << "Total blocks: " << GetTotalBlocks() << "\n";
    for (const auto& block : blocks) block.PrintSummary();
}

/**
 * @brief Returns the total number of records across all blocks.
 *
 * @return Total record count.
 */
int BlockedSequenceSet::GetTotalRecords() const {
    int total = 0;
    for (const auto& block : blocks) total += block.GetRecordCount();
    return total;
}

/**
 * @brief Returns the total number of blocks in the sequence set.
 *
 * @return Number of blocks.
 */
int BlockedSequenceSet::GetTotalBlocks() const {
    return blocks.size();
}

/**
 * @brief Searches for a record by its key (first CSV field).
 *
 * @param key The key to search for.
 * @param outRecord Reference to string to store the found record.
 * @return True if a matching record was found, false otherwise.
 */
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

/**
 * @brief Inserts a record into the appropriate block in sorted order.
 *
 * @param record The CSV record string to insert.
 *
 * This method attempts to maintain records sorted by key within each block.
 * If no suitable block exists, a new block is created at the end.
 */
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

/**
 * @brief Deletes a record by key from the sequence set.
 *
 * @param key The key identifying the record to delete.
 * @return True if deletion succeeded, false if key not found.
 */
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


/**
 * @brief Returns a copy of the internal vector of blocks.
 *
 * @return Vector of Block objects.
 */
const std::vector<Block> BlockedSequenceSet::getBlocks() const {
    return blocks;
}

/**
 * @brief Returns a flattened list of all records across all blocks.
 *
 * @return Vector of all record strings.
 */
const std::vector<std::string> BlockedSequenceSet::getRecords() const 
{
    std::vector<std::string> allRecords;
    for (const auto& block : blocks) {
        const auto& recs = block.getRecords();
        allRecords.insert(allRecords.end(), recs.begin(), recs.end());
    }
    return allRecords;
}

/**
 * @brief Dumps the physical order of all blocks.
 *
 * Physical order corresponds to the order in the vector, not RBN sequence.
 */
void BlockedSequenceSet::dumpPhysicalOrder()
{ 
    for(const Block& block: blocks) 
    {
        block.DumpContents();
        
    }
}

/**
 * @brief Dumps the logical order of all blocks based on RBN links.
 *
 * Traverses blocks by following the `nextRBN` links starting from the head.
 */
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