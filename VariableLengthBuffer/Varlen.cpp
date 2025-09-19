/**
 * @file Varlen.cpp
 * @brief Implementation of the VariableLengthBuffer class.
 * @author Ashley Bui
 * @date 2025-09-19
 * @version 1.0
 * @details This file defines member functions for the VariableLengthBuffer class,
 *          which supports variable-length records using an underlying IOBuffer.
 *          Provides methods to read/write buffers, handle headers, and manage
 *          buffer initialization and printing.
 */
// VARLEN.CPP
// Member function definitions for class VariableLengthBuffer
// Author: Ashley Bui
//
//  VariableLengthBuffer class:
//   - Extends IOBuffer for variable-length record support
//   - Provides Read/Write methods for streams
//   - Maintains buffer headers for consistency checks
//   - Can Print its contents and Initialize itself
//
// Assumptions:
//   - The underlying IOBuffer handles raw memory allocation
//   - Buffer length is represented as unsigned short in files
//   - The header string "Variable" is used for buffer identification
#include <iostream>
#include <cstring>
#include "Varlen.h"

/**
 * @brief Constructor.
 * @param maxBytes Maximum number of bytes this buffer can hold.
 * @details Initializes the buffer and calls Init() to clear contents.
 * @pre maxBytes > 0
 * @post Buffer is empty and ready for use.
 */
VariableLengthBuffer::VariableLengthBuffer(int maxBytes)
    : IOBuffer(maxBytes)
{
    Init();
}

/**
 * @brief Clears the buffer contents.
 * @details Invokes the parent IOBuffer Clear() method.
 * @pre None
 * @post Buffer is empty
 */
void VariableLengthBuffer::Clear()
{
    IOBuffer::Clear();
}

/**
 * @brief Reads a record from an input stream.
 * @param stream Input stream to read from.
 * @return Position in stream of the record, or -1 if error.
 * @details Reads the record length (unsigned short) followed by the buffer contents.
 * @pre stream is open and positioned at a record
 * @post Buffer contains the record's bytes if successful
 */
int VariableLengthBuffer::Read(istream &stream)
{
    if (stream.eof()) return -1;
    int recaddr = stream.tellg();
    Clear();
    unsigned short bufferSize;
    stream.read((char*)&bufferSize, sizeof(bufferSize));
    if (!stream.good())
    {
        stream.clear();
        return -1;
    }
    stream.read(Buffer, bufferSize);
    if (!stream.good())
    {
        stream.clear();
        return -1;
    }
    return recaddr;
}

/**
 * @brief Writes the buffer to an output stream.
 * @param stream Output stream to write to.
 * @return Position in stream of the record, or -1 if error.
 * @details Writes the buffer length (unsigned short) followed by buffer contents.
 * @pre Buffer has been initialized and contains data
 * @post Buffer is written to stream
 */
int VariableLengthBuffer::Write(ostream &stream) const
{
    int recaddr = stream.tellp();
    unsigned short bufferSize = GetUsed(); // use actual size of data
    stream.write((char*)&bufferSize, sizeof(bufferSize));
    if (!stream.good()) return -1;
    stream.write(Buffer, bufferSize);
    if (!stream.good()) return -1;
    return recaddr;
}

// header string and size
const char* headerStr = "Variable";
const int headerSize = strlen(headerStr);

/**
 * @brief Reads the buffer header from a stream.
 * @param stream Input stream to read from.
 * @return Stream position after header, or 0 if error.
 * @pre stream contains a valid buffer header
 * @post Header is verified for consistency
 */
int VariableLengthBuffer::ReadHeader(istream &stream)
{
    char str[headerSize + 1];
    int result = IOBuffer::ReadHeader(stream);
    if (!result) return 0;
    stream.read(str, headerSize);
    if (!stream.good()) return 0;
    str[headerSize] = '\0';
    if (strncmp(str, headerStr, headerSize) != 0) return 0;
    return stream.tellg();
}

/**
 * @brief Writes the buffer header to a stream.
 * @param stream Output stream to write to.
 * @return Stream position after writing header, or 0 if error.
 * @pre None
 * @post Stream contains IOBuffer header followed by "Variable" string
 */
int VariableLengthBuffer::WriteHeader(ostream &stream) const
{
    int result = IOBuffer::WriteHeader(stream);
    if (!result) return 0;
    stream.write(headerStr, headerSize);
    if (!stream.good()) return 0;
    return stream.tellp();
}

/**
 * @brief Prints the buffer contents to an output stream.
 * @param stream Output stream to print to.
 * @pre Buffer may contain valid data
 * @post Buffer contents are printed via IOBuffer::Print
 */
void VariableLengthBuffer::Print(ostream &stream) const
{
    IOBuffer::Print(stream);
}

/**
 * @brief Initializes the buffer.
 * @return 1 if successful.
 * @pre None
 * @post Buffer is cleared and ready for use
 */
int VariableLengthBuffer::Init()
{
    Clear();
    return 1;
}