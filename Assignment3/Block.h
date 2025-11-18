/**
 * @file Block.h
 * @brief Defines the Block class, which represents a fixed-size storage unit
 *        containing variable-length records for an indexed or sequentially
 *        linked file system.
 *
 * The Block class models a basic disk block or database page containing:
 *  - A block number (RBN – Relative Block Number)
 *  - Links to previous and next blocks (doubly linked list structure)
 *  - A fixed maximum byte size
 *  - A collection of variable-length records
 */
#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/**
 * @class Block
 * @brief Represents a disk block/page that stores multiple variable-length records.
 *
 * Each Block contains:
 *  - A Relative Block Number (RBN)
 *  - Pointers to the previous and next logical blocks in a chain
 *  - A vector of record strings (each typically CSV formatted)
 *  - A byte counter to track used space
 *
 * Blocks are written out in a readable text format and may be linked to form
 * a logical sequence.
 */
class Block {
private:

    /**
     * @brief Relative Block Number identifying this block uniquely.
     */
    int RBN;

    /**
     * @brief RBN of the previous block in the logical chain.
     *
     * A value of -1 indicates "no previous block."
     */

    /**
     * @brief RBN of the next block in the logical chain.
     *
     * A value of -1 indicates "no next block."
     */ 
    int prevRBN;

    /**
     * @brief RBN of the next block in the logical chain.
     *
     * A value of -1 indicates "no next block."
     */
    int nextRBN;

    /**
     * @brief The total number of bytes currently used by stored records.
     */
    int blockSize;

    /**
     * @brief Container for all variable-length records stored in this block.
     *
     * Each record is stored as a raw string and typically represents a CSV line.
     */
    int usedBytes;
    std::vector<std::string> records;

public:
    // ...existing code...
    const std::vector<std::string>& getRecords() const;

    /**
     * @brief Default constructor.
     *
     * Initializes:
     *  - RBN = -1
     *  - prevRBN = -1
     *  - nextRBN = -1
     *  - blockSize = 512 bytes (default)
     *  - usedBytes = 0
     */
    Block();

    /**
     * @brief Returns a const reference to the vector of stored records.
     *
     * @return const std::vector<std::string>& Reference to internal record list.
     */
    Block(int rbn, int maxBytesPerBlock);

    /** @brief Gets this block's relative block number. */
    int GetRBN() const { return RBN; }

    /** @brief Gets this block's previous block RBN. */
    int GetPrevRBN() const { return prevRBN; }

    /** @brief Gets this block's next block RBN. */
    int GetNextRBN() const { return nextRBN; }

    /** @brief Returns how many records this block currently stores. */
    int GetRecordCount() const { return records.size(); }

    /**
     * @brief Calculates the remaining free space in the block.
     *
     * @return Number of bytes still available before overflow.
     */
    int GetFreeSpace() const { return blockSize - usedBytes; }

    /**
     * @brief Sets the previous block link.
     *
     * @param rbn Relative block number of the previous block.
     */
    void SetPrevRBN(int rbn) { prevRBN = rbn; }

    /**
     * @brief Sets the next block link.
     *
     * @param rbn Relative block number of the next block.
     */
    void SetNextRBN(int rbn) { nextRBN = rbn; }

    /**
     * @brief Attempts to insert a record into the block.
     *
     * NOTE: This version does NOT check for overflow.
     * Future versions may use GetFreeSpace() to enforce limits.
     *
     * @param rec The record to insert (typically a CSV string).
     * @return true Always returns true (successful insert).
     */
    bool AddRecord(const std::string& rec);

    /**
     * @brief Writes the block and all records inside it to a text file.
     *
     * Format example:
     * @code
     * BLOCK 4 PREV=3 NEXT=5 COUNT=12
     * 90210,Beverly Hills,CA,...
     * 90211,Beverly Hills,CA,...
     * END_BLOCK
     * @endcode
     *
     * @param out Reference to an open std::ofstream for writing output.
     */
    void Write(std::ofstream &out) const;

    /**
     * @brief Reads a block's contents from an input stream.
     *
     * Expected format:
     *  - A header line containing RBN/PREV/NEXT information
     *  - Followed by multiple record lines
     *  - Ending with "END_BLOCK"
     *
     * @param in Input stream already positioned at first record line.
     * @param headerLine Header line describing block metadata.
     */
    void Read(std::ifstream &in, const std::string &headerLine);

    /**
     * @brief Prints a brief human-readable summary of the block.
     *
     * Can include record count, block size, or free space.
     * Often used for debugging or block map reports.
     */
    void PrintSummary() const;

    /**
     * @brief Prints all stored records in readable form.
     *
     * Useful for debugging or visual inspection of block contents.
     */
    void DumpContents() const;
};

#endif
