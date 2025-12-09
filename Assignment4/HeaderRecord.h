/**
 * @file HeaderRecord.h
 * @brief Declares the HeaderRecord class used to store and manage metadata
 *        for length-indicated data files or block-structured storage files.
 *
 * A HeaderRecord acts as the first logical record in a file and stores
 * essential metadata such as:
 *   - Block size used by the file's block storage system
 *   - Total number of data records in the file
 */
#ifndef HEADERRECORD_H
#define HEADERRECORD_H

#include <iostream>
#include <fstream>
#include <string>

/**
 * @class HeaderRecord
 * @brief Represents the metadata header stored at the beginning of a structured file.
 *
 * A HeaderRecord is responsible for maintaining information describing the file’s structure.
 * The most common fields stored include:
 *
 *   - **blockSizeBytes**: the maximum number of bytes allowed in each data block
 *   - **recordCount**: the total number of records stored in the file
 *
 * This class provides:
 *   - Constructors for default and user-defined initialization
 *   - Methods to read/write the header to a file stream
 *   - Accessor and mutator methods for header information
 *   - A print method for debugging or metadata inspection
 */
class HeaderRecord {
private:

    /**
     * @brief Maximum number of bytes per block in the file.
     *
     * This value is critical in block-structured storage systems where
     * all blocks must be the same fixed size. The header stores this
     * so future reads can interpret the file correctly.
     */
    int blockSizeBytes;

    /**
     * @brief Total number of records stored in the data file.
     *
     * This gives quick access to the number of dataset entries without
     * scanning or parsing the entire file.
     */
    int recordCount;

    /**
     * @brief Additional metadata for indexed files
     *
     * These fields may be used if the file implements an index structure.
     */
    int rootRBN; // RBN of the root block (for indexed files)

    /**
     * @brief Height of the index tree
     */
    int treeHeight; // Height of the index tree (for indexed files)

public:
    /**
     * @brief Default constructor.
     *
     * Initializes:
     *   - `blockSizeBytes = 512` (default block size)
     *   - `recordCount = 0` (empty file)
     *
     * These defaults may be overridden later using setters.
     */
    HeaderRecord();

    /**
     * @brief Constructs a header with a specific block size.
     *
     * @param blockSize Size in bytes of each block used by the file.
     *
     * The record count is initialized to zero and can later be set
     * after populating the file with data.
     */
    HeaderRecord(int blockSize);

    /**
     * @brief Retrieves the block size defined in the header.
     *
     * @return Block size in bytes.
     */
    int GetBlockSize() const { return blockSizeBytes; }

    /**
     * @brief Retrieves the number of records stored in the file.
     *
     * @return Integer count of records.
     */
    int GetRecordCount() const { return recordCount; }

    /**
     * @brief Updates the record count stored in the header.
     *
     * @param count New number of data records in the file.
     */
    void SetRecordCount(int count) { recordCount = count; }

    /**
     * @brief Writes the header record to an output file stream.
     *
     * This function writes the metadata in a structured format,
     * typically as comma-separated or length-indicated text.
     *
     * Format example:
     * @code
     * HEADER,512,1200
     * @endcode
     *
     * @param out Reference to an open std::ofstream.
     * @return `true` if write succeeds, `false` if stream is not open.
     */
    bool Write(std::ofstream &out) const;

    /**
     * @brief Reads the header record from an input file stream.
     *
     * This function expects a valid header line to be present at the
     * beginning of the file and loads internal metadata accordingly.
     *
     * @param in Reference to an open std::ifstream positioned at header.
     * @return `true` if read succeeds, `false` on failure.
     */
    bool Read(std::ifstream &in);

    /**
     * @brief Prints the header metadata in a human-readable format.
     *
     * Useful for debugging, verification, or auditing file contents.
     *
     * Example output:
     * @code
     * Block Size: 512 bytes
     * Record Count: 1200
     * @endcode
     */
    void Print() const;
};

#endif // HEADERRECORD_H