#include "HeaderRecord.h"
#include <iostream>
#include <fstream>
#include <limits>

HeaderRecord::HeaderRecord() : blockSizeBytes(512), recordCount(0) {}
HeaderRecord::HeaderRecord(int blockSize) : blockSizeBytes(blockSize), recordCount(0) {}

bool HeaderRecord::Write(std::ofstream &out) const {
    if (!out.is_open()) return false;
    out << blockSizeBytes << "," << recordCount << "\n";
    return true;
}

bool HeaderRecord::Read(std::ifstream &in) {
    if (!in.is_open()) return false;
    in >> blockSizeBytes;
    if (in.peek() == ',') in.ignore();
    in >> recordCount;
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

void HeaderRecord::Print() const {
    std::cout << "Header Record -> Block Size: " << blockSizeBytes << ", Record Count: " << recordCount << "\n";
}
