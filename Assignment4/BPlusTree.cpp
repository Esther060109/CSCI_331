#include "BPlusTree.h"
#include <iostream>

using namespace std;

// Constructor: properly initialize members in declaration order
BPlusTree::BPlusTree(const std::string& fname, int blkSize)
    : rootRBN(-1), blockSize(blkSize), filename(fname), seqSet(fname)
{
    // Nothing else to do; seqSet initialized with file
}

// Insert a record into the B+ Tree (currently delegates to seqSet)
void BPlusTree::Insert(const std::string& record)
{
    seqSet.Insert(record);
}

// Search by key
bool BPlusTree::Search(const std::string& key, std::string& outRecord)
{
    return seqSet.Search(key, outRecord);
}

// Delete by key
bool BPlusTree::Delete(const std::string& key)
{
    return seqSet.Delete(key);
}

// Print summary of tree
void BPlusTree::PrintSummary() const
{
    cout << "BPlusTree Summary:" << endl;
    cout << "Root RBN: " << rootRBN << endl;
    cout << "Block size: " << blockSize << endl;
    seqSet.PrintSummary();
}
