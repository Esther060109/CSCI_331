#include "Block.h"
#include <iostream>
#include <fstream>

using namespace std;

// Default constructor
Block::Block() : RBN(-1), prevRBN(-1), nextRBN(-1), blockSize(512), usedBytes(0) {}

// Constructor with RBN and max bytes
Block::Block(int rbn_, int maxBytes) 
    : RBN(rbn_), prevRBN(-1), nextRBN(-1), blockSize(maxBytes), usedBytes(0) {}

// Add record (no space check)
bool Block::AddRecord(const std::string& rec) {
    records.push_back(rec);
    usedBytes += rec.size();
    return true;
}

// Write block to file
void Block::Write(ofstream& out) const {
    out << "BLOCK " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN
        << " COUNT=" << records.size() << "\n";
    for (const auto& rec : records) out << rec << "\n";
    out << "END_BLOCK\n";
}

// Print block summary
void Block::PrintSummary() const {
    cout << "Block RBN: " << RBN << ", Records: " << records.size()
         << ", Free space: " << GetFreeSpace() << "\n";
}

// Dump all records
void Block::DumpContents() const {
    cout << "Block RBN " << RBN << " contents:\n";
    for (const auto& rec : records) cout << rec << "\n";
}

// Dump logical order (keys)
void Block::DumpLogicOrder() const {
    cout << "RBN " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " | ";
    for (const auto& rec : records) {
        string key = rec.substr(0, rec.find(','));
        cout << key << " ";
    }
    cout << "RBN " << RBN << endl;
}

// Return key of last record
string Block::getHighestKey() const {
    if (records.empty()) return "";
    string last = records.back();
    return last.substr(0, last.find(','));
}

// Insert record in sorted key order
void Block::InsertSorted(const std::string& rec) {
    string key = rec.substr(0, rec.find(','));
    auto it = records.begin();
    while (it != records.end() && it->substr(0, it->find(',')) < key) ++it;
    records.insert(it, rec);
    usedBytes += rec.size();
}

// Check if record fits in block
bool Block::HasSpace(const std::string& rec) const {
    return (usedBytes + static_cast<int>(rec.size())) <= blockSize;
}

// Delete record by key
bool Block::DeleteRecord(const std::string& key) {
    for (auto it = records.begin(); it != records.end(); ++it) {
        if (it->substr(0, it->find(',')) == key) {
            usedBytes -= it->size();
            records.erase(it);
            return true;
        }
    }
    return false;
}
