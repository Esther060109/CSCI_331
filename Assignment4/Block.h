#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Block {
private:
    int RBN;           // Relative Block Number
    int prevRBN;       // Previous block RBN
    int nextRBN;       // Next block RBN
    int blockSize;     // Maximum size of block in bytes
    int usedBytes;     // Bytes currently used
    std::vector<std::string> records;

public:
    // Constructors
    Block();
    Block(int rbn_, int maxBytes);

    // Accessors
    int GetRBN() const { return RBN; }
    int GetPrevRBN() const { return prevRBN; }
    int GetNextRBN() const { return nextRBN; }
    int GetRecordCount() const { return records.size(); }
    int GetFreeSpace() const { return blockSize - usedBytes; }
    const std::vector<std::string>& getRecords() const { return records; }

    // Mutators
    void SetPrevRBN(int rbn) { prevRBN = rbn; }
    void SetNextRBN(int rbn) { nextRBN = rbn; }

    // Core functions
    bool AddRecord(const std::string& rec);
    void Write(std::ofstream& out) const;
    void PrintSummary() const;
    void DumpContents() const;
    void DumpLogicOrder() const;

    std::string getHighestKey() const;
    void InsertSorted(const std::string& rec);
    bool HasSpace(const std::string& rec) const;
    bool DeleteRecord(const std::string& key);
};

#endif
