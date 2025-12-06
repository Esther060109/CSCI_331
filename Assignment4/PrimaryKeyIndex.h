#ifndef PRIMARYKEYINDEX_H
#define PRIMARYKEYINDEX_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>

/// PrimaryKeyIndex is responsible for creating and reading primary key index files.
/// Index format (text): zip,offset\n
/// - zip: decimal unsigned integer (e.g., 56301)
/// - offset: decimal byte offset (std::streampos expressed as long long)
class PrimaryKeyIndex {
public:
    /// Build an index file from a length-indicated data file.
    /// @param dataFilename input length-indicated data file (header + records)
    /// @param indexFilename output index filename to create (text)
    /// @return true on success
    static bool buildIndex(const std::string& dataFilename, const std::string& indexFilename);

    /// Load an existing index file into memory (unordered_map).
    /// @param indexFilename name of index file to read
    /// @param outIndex map to fill (zip -> offset)
    /// @return true on success
    static bool loadIndex(const std::string& indexFilename,
                          std::unordered_map<uint32_t, std::streampos>& outIndex);

    /// Save an index (map) to disk. Same format as buildIndex output.
    static bool saveIndex(const std::string& indexFilename,
                          const std::unordered_map<uint32_t, std::streampos>& index);

    /// Helper: convert streampos to printable long long
    static long long toLongLong(std::streampos p) noexcept {
        return static_cast<long long>(p);
    }
};

#endif // PRIMARYKEYINDEX_H