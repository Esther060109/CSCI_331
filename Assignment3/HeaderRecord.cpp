/**
 * @file HeaderRecord.cpp
 * @brief Implements the HeaderRecord class responsible for file metadata
 *        in block‐structured or length-indicated data files.
 *
 * The HeaderRecord stores essential information such as:
 *   - Block size (in bytes)
 *   - Number of records in the file
 *
 * Gives methods for reading, writing, and printing
 * the header in a simple comma-separated text format.
 */
#include "HeaderRecord.h"
#include <iostream>
#include <fstream>
#include <limits>

/**
 * @brief Default constructor.
 *
 * Initializes:
 *   - blockSizeBytes = 512 (default block size commonly used in block storage)
 *   - recordCount = 0 (empty file)
 */
HeaderRecord::HeaderRecord() : blockSizeBytes(512), recordCount(0) {}

/**
 * @brief Constructs a HeaderRecord with a user-defined block size.
 *
 * @param blockSize Size in bytes of each data block.
 *
 * The number of records is set to zero initially and will typically be
 * updated later once the file is populated.
 */
HeaderRecord::HeaderRecord(int blockSize) : blockSizeBytes(blockSize), recordCount(0) {}

/**
 * @brief Writes the header metadata to an output file stream.
 *
 * The header is written in a comma-separated format:
 *
 * @code
 * 512,1000
 * @endcode
 *
 * Where:
 *   - 512 = block size in bytes
 *   - 1000 = number of records in the file
 *
 * @param out Reference to an open std::ofstream where the header is written.
 * @return true if the write operation succeeds, false if the stream is not open.
 */
bool HeaderRecord::Write(std::ofstream &out) const {
    if (!out.is_open()) return false;
    out << blockSizeBytes << "," << recordCount << "\n";
    return true;
}

/**
 * @brief Reads a header record from an input file stream.
 *
 * Expects input of the form:
 *
 * @code
 * 512,1000
 * @endcode
 *
 * Reads the values safely, skipping any commas and
 * advancing to the next line after reading both numbers.
 *
 * @param in Reference to an open std::ifstream positioned at the header line.
 * @return true if read is successful, false if the stream is not open.
 */
bool HeaderRecord::Read(std::ifstream &in) {
    if (!in.is_open()) return false;

    // Read blockSizeBytes
    in >> blockSizeBytes;

    // Skip comma if present
    if (in.peek() == ',') in.ignore();

    // Read recordCount
    in >> recordCount;

    // Discard remaining characters on this line to clean stream state
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

/**
 * @brief Prints the header contents in a human-readable format.
 *
 * Useful for debugging or verifying that a file was read properly.
 *
 * Output example:
 * @code
 * Header Record -> Block Size: 512, Record Count: 1000
 * @endcode
 */
void HeaderRecord::Print() const {
    std::cout << "Header Record -> Block Size: " << blockSizeBytes << ", Record Count: " << recordCount << "\n";
}
