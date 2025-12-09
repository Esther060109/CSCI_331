/**
 * @file PrimaryKeyIndex.h
 * @brief Declares the PrimaryKeyIndex utility class for managing primary key index files.
 *
 * PrimaryKeyIndex provides static methods for creating and manipulating fast lookup
 * index files that map primary keys (ZIP codes) to byte offsets in a data file.
 * This enables O(1) random access to records without scanning the entire file.
 *
 * Index file format (text):
 * @code
 * zip,offset
 * 12345,0
 * 12346,512
 * 12347,1024
 * @endcode
 *
 * Where:
 *   - **zip**: Primary key as unsigned 32-bit integer (e.g., 56301)
 *   - **offset**: Byte position in the data file (std::streampos as long long)
 */

#ifndef PRIMARYKEYINDEX_H
#define PRIMARYKEYINDEX_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <iostream>

/**
 * @class PrimaryKeyIndex
 * @brief Utility class for building and managing primary key index files.
 *
 * All methods are static, making this a pure utility/helper class with no instance state.
 * The class supports three main operations: building an index from a data file,
 * loading an existing index into memory, and saving an index map to disk.
 */
class PrimaryKeyIndex {
public:
    /**
     * @brief Builds a primary key index file from a length-indicated data file.
     *
     * Scans a data file with length-indicated records, extracting the primary key
     * (ZIP code, first field) from each record and recording its byte offset.
     * Writes the resulting index to a text file suitable for fast lookups.
     *
     * @param dataFilename Path to input data file (header + records)
     * @param indexFilename Path to output index file to create (text format)
     * @return true if index was built successfully; false on I/O error
     *
     * @note The data file must have a valid header and use length-indicated format.
     */
    static bool buildIndex(const std::string& dataFilename, const std::string& indexFilename);

    /**
     * @brief Loads an existing index file into memory as a hash map.
     *
     * Reads a text index file and populates an unordered_map for fast O(1) lookups.
     * The map key is the ZIP code, and the value is the byte offset in the data file.
     *
     * @param indexFilename Path to index file to read
     * @param outIndex Reference to unordered_map to populate (maps ZIP -> offset)
     * @return true if load succeeds; false on I/O error
     *
     * Example:
     * @code
     * std::unordered_map<uint32_t, std::streampos> index;
     * if (PrimaryKeyIndex::loadIndex("index.txt", index)) {
     *     auto it = index.find(12345);
     *     if (it != index.end()) {
     *         std::cout << "ZIP 12345 is at offset " << it->second << std::endl;
     *     }
     * }
     * @endcode
     */
    static bool loadIndex(const std::string& indexFilename,
                          std::unordered_map<uint32_t, std::streampos>& outIndex);

    /**
     * @brief Saves an in-memory index map to disk in text format.
     *
     * Writes index entries in comma-separated format, one per line.
     * Format matches the output of buildIndex() for consistency.
     *
     * @param indexFilename Path to output file to create
     * @param index Const reference to map containing ZIP -> offset entries
     * @return true if save succeeds; false on I/O error
     */
    static bool saveIndex(const std::string& indexFilename,
                          const std::unordered_map<uint32_t, std::streampos>& index);

    /**
     * @brief Utility function to convert std::streampos to printable long long.
     *
     * std::streampos is an opaque type designed for stream positioning. This helper
     * safely converts it to long long for storage in text files or debugging output.
     *
     * @param p Stream position to convert
     * @return Byte offset as signed 64-bit integer
     *
     * @note This is intentionally noexcept as the conversion is always safe for valid positions.
     */
    static long long toLongLong(std::streampos p) noexcept {
        return static_cast<long long>(p);
    }
};

#endif // PRIMARYKEYINDEX_H