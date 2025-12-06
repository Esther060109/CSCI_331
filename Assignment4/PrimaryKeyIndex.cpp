#include "PrimaryKeyIndex.h"
#include "buffer.h" // for unpackRecord
#include <sstream>
#include <cstdlib>

/// Build index by scanning the length-indicated file, recording offsets for each record.
/// This assumes the file uses one record per newline and the first line is the header.
bool PrimaryKeyIndex::buildIndex(const std::string& dataFilename, const std::string& indexFilename) {
    std::ifstream in(dataFilename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open data file " << dataFilename << '\n';
        return false;
    }

    std::ofstream out(indexFilename, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Error: cannot create index file " << indexFilename << '\n';
        return false;
    }

    std::string line;
    // Skip header line and record its end so offsets start at first record
    if (!std::getline(in, line)) {
        std::cerr << "Error: data file appears empty: " << dataFilename << '\n';
        return false;
    }

    // Process each subsequent line; note offset is location where getline will read the record
    while (true) {
        std::streampos offset = in.tellg();
        if (!std::getline(in, line)) break; // EOF

        buffer rec;
        if (!unpackRecord(line, rec)) {
            // If unpack fails, skip (but still continue)
            std::cerr << "Warning: failed to unpack record at offset " << toLongLong(offset) << '\n';
            continue;
        }

        out << rec.zip << "," << toLongLong(offset) << '\n';
    }

    in.close();
    out.close();
    return true;
}

/// Load index file into unordered_map
bool PrimaryKeyIndex::loadIndex(const std::string& indexFilename,
                                std::unordered_map<uint32_t, std::streampos>& outIndex) {
    std::ifstream in(indexFilename);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open index file " << indexFilename << '\n';
        return false;
    }
    outIndex.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        uint32_t zip;
        long long offsetLL;
        char comma;
        if (!(ss >> zip)) continue;
        if (!(ss >> comma)) continue;
        if (!(ss >> offsetLL)) continue;
        outIndex[zip] = static_cast<std::streampos>(offsetLL);
    }
    in.close();
    return true;
}

/// Save an index map back to disk
bool PrimaryKeyIndex::saveIndex(const std::string& indexFilename,
                               const std::unordered_map<uint32_t, std::streampos>& index) {
    std::ofstream out(indexFilename, std::ios::trunc);
    if (!out.is_open()) return false;
    for (const auto& p : index) {
        out << p.first << "," << toLongLong(p.second) << '\n';
    }
    out.close();
    return true;
}