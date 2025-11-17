#include "Block.h"
#include <iostream>
#include <fstream>
using namespace std;

void Block::DumpLogicOrder() const {
    std::cout << "RBN " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " | ";
    for (const auto& record : records) {
        std::string key = record.substr(0, record.find(','));
        std::cout << key << " ";
    }
    std::cout << "RBN " << RBN << std::endl;
    // If you have an avail list, print "*available*" for available blocks
}

const std::vector<std::string>& Block::getRecords() const {
    return records;
}
#include "Block.h"

Block::Block() : RBN(-1), prevRBN(-1), nextRBN(-1), blockSize(512), usedBytes(0) {}

Block::Block(int rbn_, int maxBytes) : RBN(rbn_), prevRBN(-1), nextRBN(-1), blockSize(maxBytes), usedBytes(0) {}

bool Block::AddRecord(const std::string& rec) {
    records.push_back(rec);
    usedBytes += rec.size();
    return true;
}

void Block::Write(std::ofstream &out) const {
    out << "BLOCK " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " COUNT=" << records.size() << "\n";
    for (const auto &rec : records) out << rec << "\n";
    out << "END_BLOCK\n";
}

void Block::PrintSummary() const {
    //std::cout << "Block RBN: " << RBN << ", Records: " << records.size() << "\n";
}

void Block::DumpContents() const {
    std::cout << "RBN " << RBN << " PREV=" << prevRBN << " NEXT=" << nextRBN << " | ";
    for (const auto& record : records) {
        // Assuming the key (Zip Code) is the first field in the record
        std::string key = record.substr(0, record.find(','));
        std::cout << key << " ";
    }
    std::cout << "RBN " << RBN << std::endl;
    // If you have an avail list, print "*available*" for available blocks
}
