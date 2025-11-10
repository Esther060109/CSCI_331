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
