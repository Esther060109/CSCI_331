/**
 * @file Block.cpp
 * @brief Implementation of the Block class used to store variable-length records
 *        inside fixed-size blocks for indexed storage systems.
 *
 * This module provides functionality for:
 *  - Creating blocks containing multiple CSV-formatted records
 *  - Writing block contents to disk
 *  - Tracking logical neighbors via RBN links (Prev/Next pointers)
 *  - Dumping block contents in logical order
 *
 * Each Block simulates a fixed-size storage unit often used in
 * indexed file organizations, database page structures, or B+ tree nodes.
 */
#include "Block.h"
#include <iostream>
#include <fstream>
using namespace std;

/**
 * @brief Prints the block contents in a human-readable form showing
 *        the logical order (linked list form) of the block.
 *
 * This method prints:
 *   - RBN (Relative Block Number)
 *   - Previous and next RBN values
 *   - Keys extracted from each record (first field before the comma)
 *
 * Useful for debugging the physical/logical organization of the block chain.
 *
 * Example output:
 * @code
 * RBN 3 PREV=2 NEXT=4 | 90210 90211 90212 RBN 3
 * @endcode
 */
void Block::DumpLogicOrder() const {
    std::cout << "RBN " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " | ";
    
    // Print only the key (the part before the first comma
    for (const auto& record : records) {
        std::string key = record.substr(0, record.find(','));
        std::cout << key << " ";
    }
    std::cout << "RBN " << RBN << std::endl;
    // NOTE: If you are implementing an "avail list", you would
    // print "*available*" here if this block is in the free list
}

/**
 * @brief Returns a const reference to the internal vector of records.
 *
 * This avoids copying the vector and allows read-only access.
 *
 * @return const std::vector<std::string>& A reference to the block's records.
 */
const std::vector<std::string>& Block::getRecords() const {
    return records;
}
#include "Block.h"

/**
 * @brief Default constructor. Initializes an empty block with standard defaults.
 *
 * - RBN = -1 (invalid)
 * - prevRBN = -1
 * - nextRBN = -1
 * - blockSize = 512 bytes (default)
 * - usedBytes = 0
 */
Block::Block() : RBN(-1), prevRBN(-1), nextRBN(-1), blockSize(512), usedBytes(0) {}

/**
 * @brief Overloaded constructor specifying this block's RBN and max storage size.
 *
 * @param rbn_ The block's relative block number.
 * @param maxBytes The maximum number of bytes this block may store.
 */
Block::Block(int rbn_, int maxBytes) : RBN(rbn_), prevRBN(-1), nextRBN(-1), blockSize(maxBytes), usedBytes(0) {}

/**
 * @brief Attempts to insert a record into the block.
 *
 * NOTE: Currently this version does NOT check for space overflow.
 *       It simply stores the record and increases usedBytes.
 *
 * @param rec The CSV-formatted record string to insert.
 * @return true Always returns true (success).
 *
 * Future enhancement: Check (usedBytes + rec.size() <= blockSize)
 */
bool Block::AddRecord(const std::string& rec) {
    records.push_back(rec);
    usedBytes += rec.size();
    return true;
}

/**
 * @brief Writes the block contents to an output file stream in a readable format.
 *
 * Output Format:
 * @code
 * BLOCK 3 PREV=2 NEXT=4 COUNT=5
 * 90210,Beverly Hills,CA,...
 * 90211,Beverly Hills,CA,...
 * END_BLOCK
 * @endcode
 *
 * This format is NOT binary; it is human-readable for debugging.
 *
 * @param out Reference to an already-open std::ofstream.
 */
void Block::Write(std::ofstream &out) const {
    out << "BLOCK " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " COUNT=" << records.size() << "\n";
    for (const auto &rec : records) out << rec << "\n";
    out << "END_BLOCK\n";
}

/**
 * @brief Prints a short summary of the block such as RBN and record count.
 *
 * This is mainly for debugging and can be expanded to show more information.
 */
void Block::PrintSummary() const {
    // Uncomment to enable summary output:
    //std::cout << "Block RBN: " << RBN << ", Records: " << records.size() << "\n";
}
