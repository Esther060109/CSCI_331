/**
 * @file BPlusTree.cpp
 * @brief Implementation of the BPlusTree class for managing a static B+ tree index.
 */

#include "BPlusTree.h"
#include <iostream>
#include <algorithm>
#include <sstream>

using namespace std;

/**
 * @brief Constructs a BPlusTree with specified filename and block size.
 *
 * @param fname Path to the file where blocks will be written
 * @param blkSize Size of each block in bytes (e.g., 512)
 */
BPlusTree::BPlusTree(const std::string& fname, int blkSize)
    : rootRBN(-1), blockSize(blkSize), filename(fname), seqSet(fname)
{
    // Initialize the blocked sequence set for the B+ tree
}

/**
 * @brief Builds the complete static B+ tree index.
 *
 * After all records have been inserted into the sequence set,
 * this method creates the index structure on top of it.
 */
void BPlusTree::BuildStaticIndex()
{
    // Write the sequence set to file first
    seqSet.WriteToFile();
    
    // For now, the sequence set itself serves as the leaf level (RBN = 0)
    rootRBN = 0;
    
    // In a full B+ tree implementation, you would:
    // 1. Build intermediate index blocks from the leaf blocks
    // 2. Recursively build higher levels until a single root is reached
    // 3. Update metadata about tree height and structure
    
    std::cout << "[BPlusTree::BuildStaticIndex] Tree built with root RBN = " << rootRBN << std::endl;
}

/**
 * @brief Inserts a record into the B+ tree.
 *
 * @param record String record to insert (comma-separated fields)
 */
void BPlusTree::Insert(const std::string& record)
{
    // Add record to the sequence set
    seqSet.AddRecord(record);
    
    // Note: In a dynamic B+ tree, we would check for block overflow and rebalance
    // For this static implementation, we just add to the sequence set
}

/**
 * @brief Searches for a record by primary key.
 *
 * @param key Primary key value to search for
 * @param outRecord Reference to string where matching record is stored
 * @return true if record found; false if not found
 */
bool BPlusTree::Search(const std::string& key, std::string& outRecord)
{
    // For a static tree with sequence set as root, search the sequence set directly
    // In a full B+ tree, this would traverse from root through index blocks to find
    // the appropriate leaf block, then search within that leaf.
    
    // For now, we search all records in the sequence set
    const auto& records = seqSet.getRecords();
    
    for (const auto& recStr : records) {
        // Extract ZIP code (first field before comma)
        size_t commaPos = recStr.find(',');
        if (commaPos != string::npos) {
            string recordKey = recStr.substr(0, commaPos);
            if (recordKey == key) {
                outRecord = recStr;
                return true;
            }
        }
    }
    
    return false;
}

/**
 * @brief Deletes a record by primary key.
 *
 * @param key Primary key value of the record to delete
 * @return true if record found and deleted; false if not found
 */
bool BPlusTree::Delete(const std::string& key)
{
    // This would require modifying the BlockedSequenceSet to support deletion
    // For now, return false
    return false;
}

/**
 * @brief Prints a summary of the tree structure.
 */
void BPlusTree::PrintSummary() const
{
    std::cout << "\n=== B+ Tree Summary ===" << std::endl;
    std::cout << "Root RBN: " << rootRBN << std::endl;
    std::cout << "Block Size: " << blockSize << " bytes" << std::endl;
    std::cout << "Total Records: " << seqSet.getRecords().size() << std::endl;
    std::cout << "========================\n" << std::endl;
}

/**
 * @brief Searches for all records matching a given state abbreviation.
 *
 * @param state Two-letter state code to search for
 * @param outRecords Reference to vector where matching records are stored
 */
void BPlusTree::SearchByState(const std::string& state, std::vector<std::string>& outRecords) const
{
    // Search all records in the sequence set for matching state
    const auto& records = seqSet.getRecords();
    
    for (const auto& recStr : records) {
        // Parse: ZIP,PLACE,STATE,COUNTY,LAT,LON
        // State is the 3rd field
        int fieldCount = 0;
        size_t start = 0;
        size_t end = 0;
        std::string currentField;
        
        for (size_t i = 0; i <= recStr.length(); ++i) {
            if (i == recStr.length() || recStr[i] == ',') {
                if (fieldCount == 2) {  // STATE field is the 3rd (index 2)
                    currentField = recStr.substr(start, i - start);
                    if (currentField == state) {
                        outRecords.push_back(recStr);
                    }
                    break;
                }
                fieldCount++;
                start = i + 1;
            }
        }
    }
}

/**
 * @brief Dumps the complete B+ tree structure to an output stream.
 *
 * @param out Reference to output stream
 */
void BPlusTree::DumpTree(std::ostream& out) const
{
    out << "\n=== B+ Tree Structure Dump ===" << std::endl;
    out << "Root RBN: " << rootRBN << std::endl;
    out << "Block Size: " << blockSize << " bytes" << std::endl;
    out << "\n--- Leaf Level (Sequence Set) ---" << std::endl;
    
    const auto& records = seqSet.getRecords();
    out << "Total Records: " << records.size() << std::endl;
    
    // Show first few records as sample
    int count = 0;
    for (const auto& rec : records) {
        out << "  " << rec << std::endl;
        count++;
        if (count >= 10) {
            if (records.size() > 10) {
                out << "  ... (" << (records.size() - 10) << " more records)" << std::endl;
            }
            break;
        }
    }
    
    out << "\n================================" << std::endl;
}
