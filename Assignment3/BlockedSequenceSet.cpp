#include "BlockedSequenceSet.h"

BlockedSequenceSet::BlockedSequenceSet(const std::string& fname) : filename(fname) {
    blocks.clear();
}

void BlockedSequenceSet::AddRecord(const std::string& rec) {
    if (blocks.empty() || blocks.back().GetFreeSpace() < static_cast<int>(rec.size())) {
        Block newBlock(blocks.size(), 512);
        blocks.push_back(newBlock);
    }
    blocks.back().AddRecord(rec);
}

void BlockedSequenceSet::WriteToFile() {
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Cannot open file: " << filename << "\n";
        return;
    }
    for (const auto& block : blocks) block.Write(out);
    out.close();
}

void BlockedSequenceSet::PrintSummary() const {
    std::cout << "BlockedSequenceSet Summary:\n";
    std::cout << "File: " << filename << "\n";
    std::cout << "Total records: " << GetTotalRecords() << "\n";
    std::cout << "Total blocks: " << GetTotalBlocks() << "\n";
    for (const auto& block : blocks) block.PrintSummary();
}

int BlockedSequenceSet::GetTotalRecords() const {
    int total = 0;
    for (const auto& block : blocks) total += block.GetRecordCount();
    return total;
}

int BlockedSequenceSet::GetTotalBlocks() const {
    return blocks.size();
}
