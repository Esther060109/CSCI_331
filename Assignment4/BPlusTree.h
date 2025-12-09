#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <string>
#include <fstream>
#include <vector>
#include <ostream> // for std::ostream
#include "BlockedSequenceSet.h"

class BPlusTree {
private:
    int rootRBN;              // RBN of root node
    int blockSize;            // size of each block
    std::string filename;     // file containing both sequence set + index blocks

    BlockedSequenceSet seqSet;  // logical storage of blocks

public:
    // Constructor: specify filename and block size
    BPlusTree(const std::string& fname, int blkSize);

    void BuildStaticIndex(); 

    // Insert a record into the B+ Tree
    void Insert(const std::string& record);

    // Search for a record by key (first field)
    bool Search(const std::string& key, std::string& outRecord);

    // Delete a record by key
    bool Delete(const std::string& key);

    // Print tree structure or summary
    void PrintSummary() const;

    // Search all records for a given state abbreviation (e.g., "MN")
    void SearchByState(const std::string& state, std::vector<std::string>& outRecords) const;

    // Access underlying blocked sequence set
    BlockedSequenceSet& GetSequenceSet() { return seqSet; }

    // Dump the B+ tree structure
    void DumpTree(std::ostream& out) const;
};

#endif // BPLUSTREE_H
