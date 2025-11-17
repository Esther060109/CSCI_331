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


void Block::InsertSorted(const std::string& rec)
{
    std::string key = rec.substr(0, rec.find(','));

    auto it = records.begin();
    for (; it != records.end(); ++it)
    {
        std::string existingKey = it->substr(0, it->find(','));
        if (key < existingKey)
            break;
    }

    records.insert(it, rec);
    usedBytes += rec.size();
}

bool Block::DeleteRecord(const std::string& key)
{
    for (auto it = records.begin(); it != records.end(); ++it)
    {
        std::string recKey = it->substr(0, it->find(','));

        if (recKey == key)
        {
            usedBytes -= it->size();
            records.erase(it);
            return true;
        }
    }
    return false;  // not found
}

std::string Block::getHighestKey() const
{
    if (records.empty()) return "";

    const std::string& rec = records.back();
    return rec.substr(0, rec.find(','));
}

bool Block::HasSpace(const std::string& rec) const {
    return (usedBytes + rec.size() <= blockSize);
}
