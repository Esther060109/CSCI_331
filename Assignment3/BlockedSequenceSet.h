/**
 * @file BlockedSequenceSet.h
 * @brief Defines the BlockedSequenceSet class, which manages a sequence of
 *        fixed-size blocks used to store variable-length records.
 *
 * Class implements a simplified version of a Blocked Sequence Set (BSS),
 * a classic storage structure used in file organizations such as:
 *   - Indexed Sequence Sets
 *   - ISAM-like systems
 *   - Overflow-protected block chains
 *
 * The structure works by grouping multiple logical records into fixed-size
 * blocks, allowing efficient reading and writing operations.
 */
#ifndef BLOCKEDSEQUENCESET_H
#define BLOCKEDSEQUENCESET_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Block.h"

/**
 * @class BlockedSequenceSet
 * @brief Manages an ordered collection of fixed-size blocks that store
 *        variable-length string records.
 *
 * A BlockedSequenceSet behaves like a sequence of linked disk blocks.
 * Each block has:
 *   - A Record Block Number (RBN)
 *   - A previous/next RBN for logical order
 *   - A list of records
 *   - Limited free space
 *
 * Example usage:
 * @code
 * BlockedSequenceSet bss("zipcodeBlocks.dat");
 * bss.AddRecord("12345,HELENA,MT,LEWIS AND CLARK,46.5927,-112.0361");
 * bss.WriteToFile();
 * @endcode
 */
class BlockedSequenceSet {
private:
    /**
     * @brief Vector holding the logical sequence of blocks.
     *
     * Blocks are stored in RBN order (i.e., element index in the vector
     * corresponds to block number). This keeps the model simple and enables
     * predictable sequential storage.
     */
    std::vector<Block> blocks;

    /**
     * @brief The name of the file to which this BlockedSequenceSet will be written.
     *
     * Used by WriteToFile() when serializing the block contents.
     */
    std::string filename;

public:
    /**
     * @brief Constructs a BlockedSequenceSet and associates it with a target filename.
     *
     * @param fname Name of output file used when writing blocks to disk.
     *
     * The constructor initializes the block container but performs no I/O.
     */
    BlockedSequenceSet(const std::string& fname);

    /**
     * @brief Adds a new record to the sequence set.
     *
     * If the last block does not have enough space, a new block is created.
     *
     * @param rec The record string to insert into the Blocked Sequence Set.
     */
    void AddRecord(const std::string& rec);

    /**
     * @brief Writes all blocks to the configured output file.
     *
     * The output is written in a human-readable text format defined by Block::Write().
     *
     * @note Existing file contents are overwritten.
     */
    void WriteToFile();

    /**
     * @brief Prints a summary of all blocks in the sequence set.
     *
     * This includes:
     *   - Number of blocks
     *   - Number of records per block
     *   - Logical ordering via RBN links
     */
    void PrintSummary() const;

    /**
     * @brief Returns the total number of logical records stored across all blocks.
     *
     * @return Total record count aggregated from all blocks.
     */
    int GetTotalRecords() const;

    /**
     * @brief Returns the total number of blocks used in the sequence set.
     *
     * @return Number of Block objects currently stored.
     */
    int GetTotalBlocks() const;
};

#endif  // BLOCKEDSEQUENCESET_H
