#ifndef HEADERRECORD_H
#define HEADERRECORD_H

#include <iostream>
#include <fstream>
#include <string>

class HeaderRecord {
private:
    int blockSizeBytes;
    int recordCount;

public:
    HeaderRecord();
    HeaderRecord(int blockSize);

    int GetBlockSize() const { return blockSizeBytes; }
    int GetRecordCount() const { return recordCount; }
    void SetRecordCount(int count) { recordCount = count; }

    bool Write(std::ofstream &out) const;
    bool Read(std::ifstream &in);
    void Print() const;
};

#endif
