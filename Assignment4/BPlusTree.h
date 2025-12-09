/**
 * @file BPlusTree.h
 * @brief Declares the BPlusTree class for managing a static B+ tree index structure.
 *
 * A B+ tree is a self-balancing tree data structure that maintains sorted records
 * across multiple levels, enabling efficient range queries and logarithmic search.
 * This implementation builds a static B+ tree (entire structure created after all
 * inserts are complete) stored in fixed-size blocks on disk.
 *
 * The B+ tree consists of:
 *   - **Leaf blocks** (sequence set): Linked blocks containing actual records
 *   - **Index blocks**: Non-leaf nodes containing search keys and child RBNs
 *   - **Root block**: Entry point for tree traversal
 */

#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <string>
#include <fstream>
#include <vector>
#include <ostream> // for std::ostream
#include "BlockedSequenceSet.h"

/**
 * @class BPlusTree
 * @brief Implements a static B+ tree index for efficient record retrieval.
 *
 * The BPlusTree provides a hierarchical index structure over a BlockedSequenceSet
 * (the leaf level). Records are inserted into the sequence set first, then the
 * entire tree structure is built once via BuildStaticIndex().
 *
 * Key features:
 *   - **Static building**: Tree is built once, not dynamically rebalanced
 *   - **Disk-based storage**: All blocks persisted to a file
 *   - **Sorted access**: Leaf blocks are linked for sequential traversal
 *   - **Logarithmic search**: O(log n) via tree traversal from root to leaf
 *
 * Example usage:
 * @code
 * BPlusTree tree("zipcodes.dat", 512);
 * // Add records to sequence set
 * tree.GetSequenceSet().AddRecord("12345,Helena,MT,...");
 * tree.GetSequenceSet().AddRecord("12346,Missoula,MT,...");
 * // Build index after all records inserted
 * tree.BuildStaticIndex();
 * // Search using tree traversal
 * std::string result;
 * tree.Search("12345", result);
 * @endcode
 */
class BPlusTree {
private:
    int rootRBN;              ///< Record Block Number of the root index block
    int blockSize;            ///< Size of each block in bytes (typically 512)
    std::string filename;     ///< File path for persistent storage of all blocks
    BlockedSequenceSet seqSet;  ///< Sequence set (leaf level) containing all records

public:
    /**
     * @brief Constructs a BPlusTree with specified filename and block size.
     *
     * @param fname Path to the file where blocks will be written
     * @param blkSize Size of each block in bytes (e.g., 512)
     *
     * Initializes:
     *   - filename = fname
     *   - blockSize = blkSize
     *   - rootRBN = -1 (tree not yet built)
     *   - seqSet with empty blocks
     */
    BPlusTree(const std::string& fname, int blkSize);

    /**
     * @brief Builds the complete static B+ tree index.
     *
     * This method must be called after all records have been added to the
     * sequence set. It creates index blocks in a bottom-up fashion, organizing
     * leaf blocks into a hierarchical tree structure according to B+ tree rules.
     *
     * @note This is the key step that transforms a flat BlockedSequenceSet into
     *       a multi-level B+ tree suitable for efficient searching.
     */
    void BuildStaticIndex();

    /**
     * @brief Inserts a record into the B+ tree.
     *
     * For a static B+ tree, this typically delegates to the sequence set and
     * marks the tree as needing rebuilding before searches.
     *
     * @param record String record to insert (comma-separated fields)
     */
    void Insert(const std::string& record);

    /**
     * @brief Searches for a record by primary key using tree traversal.
     *
     * Begins at the root block and recursively traverses index blocks until
     * reaching the appropriate leaf block, then searches within the leaf.
     *
     * @param key Primary key value to search for (e.g., ZIP code)
     * @param outRecord Reference to string where matching record is stored
     * @return true if record found; false if not found
     */
    bool Search(const std::string& key, std::string& outRecord);

    /**
     * @brief Deletes a record by primary key.
     *
     * @param key Primary key value of the record to delete
     * @return true if record found and deleted; false if not found
     */
    bool Delete(const std::string& key);

    /**
     * @brief Prints a summary of the tree structure to console.
     *
     * Displays metadata such as:
     *   - Root RBN
     *   - Block size
     *   - Total record count
     *   - Tree height
     */
    void PrintSummary() const;

    /**
     * @brief Searches for all records matching a given state abbreviation.
     *
     * Performs a full scan of the leaf blocks (sequence set) to find all records
     * with the specified state code in their state field.
     *
     * @param state Two-letter state code to search for (e.g., "MT", "MN")
     * @param outRecords Reference to vector where matching records are stored
     */
    void SearchByState(const std::string& state, std::vector<std::string>& outRecords) const;

    /**
     * @brief Provides access to the underlying BlockedSequenceSet.
     *
     * Useful for directly manipulating the leaf level (sequence set) and
     * adding records before tree construction.
     *
     * @return Reference to the internal BlockedSequenceSet object
     */
    BlockedSequenceSet& GetSequenceSet() { return seqSet; }

    /**
     * @brief Dumps the complete B+ tree structure to an output stream.
     *
     * Produces a detailed hierarchical view showing:
     *   - Root block and its key entries
     *   - Index block levels and their children
     *   - Leaf blocks and their records
     *
     * @param out Reference to output stream (e.g., std::cout or file stream)
     */
    void DumpTree(std::ostream& out) const;
};

#endif // BPLUSTREE_H
