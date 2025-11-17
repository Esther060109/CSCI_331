#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Block {
private:
    int RBN;
    int prevRBN;
    int nextRBN;
    int blockSize;
    int usedBytes;
    std::vector<std::string> records;

public:
    // ...existing code...
    const std::vector<std::string>& getRecords() const;
    Block();
    Block(int rbn, int maxBytesPerBlock);

    int GetRBN() const { return RBN; }
    int GetPrevRBN() const { return prevRBN; }
    int GetNextRBN() const { return nextRBN; }
    int GetRecordCount() const { return records.size(); }
    int GetFreeSpace() const { return blockSize - usedBytes; }

    void SetPrevRBN(int rbn) { prevRBN = rbn; }
    void SetNextRBN(int rbn) { nextRBN = rbn; }

    bool AddRecord(const std::string& rec);
    void Write(std::ofstream &out) const;
    void Read(std::ifstream &in, const std::string &headerLine);
    void PrintSummary() const;
    void DumpContents() const;
};

#endif
