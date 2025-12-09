/**
 * @file Block.h
 * @brief Declares the Block class for managing fixed-size blocks in a file-based storage system.
 *
 * A Block is a fundamental unit of storage that holds a collection of variable-length
 * records within a fixed maximum size (typically 512 bytes). Blocks can function as either
 * leaf blocks (storing records) or index blocks (storing key-RBN pairs).
 *
 * Blocks are linked together via RBN references (prevRBN, nextRBN) to form a doubly-linked
 * chain, enabling sequential traversal without random access.
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

/**
 * @enum BlockType
 * @brief Enumeration to distinguish block functionality.
 *
 * A Block can serve two distinct purposes depending on its type:
 *   - **LEAF_BLOCK**: Stores actual data records in a sequence set
 *   - **INDEX_BLOCK**: Stores index entries (key-RBN pairs) in the B+ tree index
 */
enum BlockType
{
    LEAF_BLOCK,  ///< Block stores data records
    INDEX_BLOCK  ///< Block stores index entries (key, RBN pairs)
};

/**
 * @class Block
 * @brief Represents a fixed-size block in a block-structured storage system.
 *
 * A Block holds up to @c blockSize bytes of data, organized as a sequence of
 * variable-length records. When used as a leaf block, it stores complete records.
 * When used as an index block, it stores (key, RBN) pairs for navigating the index.
 *
 * Each Block maintains:
 *   - A Record Block Number (RBN) for identification in a file
 *   - Links to previous and next blocks (prevRBN, nextRBN) for sequencing
 *   - A vector of records and tracking of available space
 *   - A block type indicator (LEAF_BLOCK or INDEX_BLOCK)
 *
 * Example usage (leaf block):
 * @code
 * Block block(0, 512);  // RBN=0, 512-byte capacity
 * block.AddRecord("12345,Helena,MT,Lewis and Clark");
 * block.SetNextRBN(1);
 * @endcode
 */
class Block {
private:
    int RBN;           ///< Relative Block Number (unique identifier within file)
    int prevRBN;       ///< RBN of previous block in logical sequence (-1 if none)
    int nextRBN;       ///< RBN of next block in logical sequence (-1 if none)
    int blockSize;     ///< Maximum capacity of block in bytes
    int usedBytes;     ///< Bytes currently occupied by records
    
    std::vector<std::string> records;  ///< Records stored in this block
   
    BlockType type;    ///< Indicates whether block stores records (LEAF) or keys (INDEX)

public:
    /**
     * @brief Default constructor.
     *
     * Initializes a Block with default values:
     *   - RBN = -1 (unassigned)
     *   - blockSize = 512 bytes
     *   - prevRBN = nextRBN = -1 (unlinked)
     *   - usedBytes = 0 (empty)
     *   - type = LEAF_BLOCK
     */
    Block();

    /**
     * @brief Constructs a Block with specified RBN and capacity.
     *
     * @param rbn_ Record Block Number to assign to this block
     * @param maxBytes Maximum block size in bytes (e.g., 512)
     *
     * Initializes:
     *   - RBN to @c rbn_
     *   - blockSize to @c maxBytes
     *   - prevRBN = nextRBN = -1 (unlinked)
     *   - usedBytes = 0 (empty)
     *   - type = LEAF_BLOCK
     */
    Block(int rbn_, int maxBytes);

    /**
     * @name Accessor Methods
     * @{
     */

    /**
     * @brief Retrieves the Record Block Number.
     * @return Block's RBN
     */
    int GetRBN() const { return RBN; }

    /**
     * @brief Retrieves the RBN of the previous block in the sequence.
     * @return Previous block's RBN (-1 if this is the first block)
     */
    int GetPrevRBN() const { return prevRBN; }

    /**
     * @brief Retrieves the RBN of the next block in the sequence.
     * @return Next block's RBN (-1 if this is the last block)
     */
    int GetNextRBN() const { return nextRBN; }

    /**
     * @brief Retrieves the number of records currently stored.
     * @return Count of records in this block
     */
    int GetRecordCount() const { return records.size(); }

    /**
     * @brief Retrieves the number of free bytes remaining.
     * @return blockSize - usedBytes
     */
    int GetFreeSpace() const { return blockSize - usedBytes; }

    /**
     * @brief Provides const access to all records in this block.
     * @return Const reference to the records vector
     */
    const std::vector<std::string>& getRecords() const { return records; }

    /**
     * @brief Retrieves the block type (LEAF or INDEX).
     * @return Current BlockType value
     */
    BlockType GetType() const { return type; }

    /** @} */

    /**
     * @name Mutator Methods
     * @{
     */

    /**
     * @brief Sets the RBN of the previous block.
     * @param rbn RBN of the previous block (-1 for none)
     */
    void SetPrevRBN(int rbn) { prevRBN = rbn; }

    /**
     * @brief Sets the RBN of the next block.
     * @param rbn RBN of the next block (-1 for none)
     */
    void SetNextRBN(int rbn) { nextRBN = rbn; }

    /**
     * @brief Sets the block type (LEAF_BLOCK or INDEX_BLOCK).
     * @param t The desired BlockType
     */
    void SetType(BlockType t) { type = t; }

    /** @} */

    /**
     * @name Core Operations
     * @{
     */

    /**
     * @brief Attempts to add a record to this block.
     *
     * The record is added only if sufficient free space is available.
     * Records are stored in sorted order by their primary key (first field).
     *
     * @param rec String record to insert
     * @return true if record was added; false if insufficient space
     *
     * @note Records must follow a comma-separated format with the primary key
     *       (ZIP code) as the first field for proper sorting.
     */
    bool AddRecord(const std::string& rec);

    /**
     * @brief Writes the block's contents to an output file stream.
     *
     * Format includes metadata (RBN, type, record count) followed by each record.
     * Example:
     * @code
     * [Block RBN=0 TYPE=LEAF COUNT=3]
     * 12345,Helena,MT,...
     * 12346,Missoula,MT,...
     * @endcode
     *
     * @param out Reference to open std::ofstream
     */
    void Write(std::ofstream& out) const;

    /**
     * @brief Prints a summary of block metadata to console.
     *
     * Displays: RBN, block size, used bytes, record count, and links.
     */
    void PrintSummary() const;

    /**
     * @brief Dumps all block contents to console for debugging.
     *
     * Shows metadata followed by each record stored.
     */
    void DumpContents() const;

    /**
     * @brief Dumps records in logical (sorted) order to console.
     */
    void DumpLogicOrder() const;

    /** @} */

    /**
     * @name Utility Methods
     * @{
     */

    /**
     * @brief Retrieves the primary key (first field) of the highest-valued record.
     *
     * Used during B+ tree construction to determine upper bounds for index entries.
     *
     * @return String containing the highest key, or empty string if block is empty
     */
    std::string getHighestKey() const;

    /**
     * @brief Inserts a record maintaining sorted order by primary key.
     *
     * Used internally to keep records sorted for efficient searching.
     *
     * @param rec Record string to insert
     */
    void InsertSorted(const std::string& rec);

    /**
     * @brief Checks if a record of the given length can fit in free space.
     *
     * @param rec Record string to check
     * @return true if the block has sufficient space; false otherwise
     */
    bool HasSpace(const std::string& rec) const;

    /**
     * @brief Removes a record by its primary key.
     *
     * @param key Primary key (ZIP code) of the record to delete
     * @return true if record found and deleted; false if not found
     */
    bool DeleteRecord(const std::string& key);

    /** @} */
};

#endif // BLOCK_H
