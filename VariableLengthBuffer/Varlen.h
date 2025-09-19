/**
 * @file Varlen.h
 * @brief Declaration of the VariableLengthBuffer class.
 * @author Ashley Bui
 * @date 2025-09-19
 * @version 1.0
 * @details This file declares the VariableLengthBuffer class, which extends IOBuffer
 *          to support variable-length records. Provides methods for reading/writing
 *          buffers, handling headers, packing/unpacking CSV fields, and printing
 *          buffer contents.
 */
//
// VARLEN.H
// Declaration of class VariableLengthBuffer
// Author: Ashley Bui
//
// VariableLengthBuffer class:
//   - Abstract class designed to support variable-length records
//   - Supports packing of fixed-length, delimited, and length-prefixed data
//   - Maintains header string for stream consistency
//   - Can read/write to/from streams and print buffer contents
//
// Assumptions:
//   - IOBuffer base class is implemented and provides basic buffer management
//   - Buffer length is stored as unsigned short in stream files
//   - Header string "Variable" is used to identify buffer type

#ifndef VARLEN_H
#define VARLEN_H

#include <iostream>
#include <cstring>   
#include "IOBuffer.h"

using namespace std;

/**
 * @class VariableLengthBuffer
 * @brief Extends IOBuffer for variable-length record support.
 * @details Provides methods to read/write buffers, handle headers, 
 *          pack different types of fields, and print buffer contents.
 */
class VariableLengthBuffer : public IOBuffer
{
public:
    /**
     * @brief Constructor.
     * @param MaxBytes Maximum number of bytes for this buffer (default 1000).
     * @post Buffer is initialized and cleared.
     */
    VariableLengthBuffer(int MaxBytes = 1000);
    
    /**
     * @brief Copy constructor.
     * @param buffer Another VariableLengthBuffer object to copy.
     * @post Buffer contents are copied from the source.
     */
    VariableLengthBuffer(const VariableLengthBuffer &buffer)
        : IOBuffer(buffer) {}

    /**
     * @brief Clears the buffer.
     * @post Buffer is empty.
     */    
    void Clear();     
    
    /**
     * @brief Reads a record from an input stream.
     * @param stream Input stream to read from.
     * @return Position in stream of the record, or 0 if error.
     */
    int Read(istream &); 
    
    /**
     * @brief Writes the buffer to an output stream.
     * @param stream Output stream to write to.
     * @return Position in stream after writing, or 0 if error.
     */
    int Write(ostream &) const; 

    /**
     * @brief Reads the buffer header from a stream.
     * @param stream Input stream containing the header.
     * @return Stream position after reading header, or 0 if error.
     */
    int ReadHeader(istream &);

    /**
     * @brief Writes the buffer header to a stream.
     * @param stream Output stream to write the header.
     * @return Stream position after writing header, or 0 if error.
     */
    int WriteHeader(ostream &) const; 

    // Packing methods for different field types (CSV/variable-length)

    /**
     * @brief Packs fixed-length data into the buffer.
     * @param data Pointer to data to pack.
     * @param size Number of bytes to pack.
     * @return Number of bytes packed, or -1 on error.
     */
    int PackFixLen(void *, int);

    /**
     * @brief Packs delimited data into the buffer.
     * @param data Pointer to data to pack.
     * @param size Number of bytes to pack.
     * @return Number of bytes packed, or -1 on error.
     */
    int PackDelimeted(void *, int);

    /**
     * @brief Packs length-prefixed data into the buffer.
     * @param data Pointer to data to pack.
     * @param size Number of bytes to pack.
     * @return Number of bytes packed, or -1 on error.
     */
    int PackLength(void *, int);

    /**
     * @brief Prints the buffer contents to a stream.
     * @param stream Output stream.
     * @post Buffer contents are written to the stream.
     */
    void Print(ostream &) const;

    /**
     * @brief Returns the current size of the buffer.
     * @return Number of bytes currently used in the buffer.
     */
    int SizeOfBuffer() const;    
    
    /**
     * @brief Initializes the buffer.
     * @return 1 if successful, 0 otherwise.
     * @post Buffer is cleared and ready for use.
     */
    int Init();
};

#endif
