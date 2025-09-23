#include <iostream>
#include <fstream>
#include "..\IOBuffer\Iobuffer.h"
#include "Buffile.h"

using namespace std;

/**
 * @brief Constructor
 * @param from Reference to an IOBuffer object to associate with this BufferFile.
 * @post BufferFile is initialized with the provided IOBuffer.
 */
BufferFile::BufferFile(IOBuffer & from) : Buffer(from) {}

BufferFile::~BufferFile() 
{ 
    if(File.is_open()) 
        File.close(); 
}

/**
 * @brief Destructor
 * @post File is closed if open.
 */
int BufferFile::Open(const char * filename, std::ios::openmode mode)
{
    // Open the file with given mode (add binary)
    File.open(filename, mode | std::ios::binary);
    if (!File.good()) return 0;

    // Ensure both get/put are at start before reading header
    File.seekg(0, std::ios::beg);
    File.seekp(0, std::ios::beg);

    HeaderSize = ReadHeader();
    if (HeaderSize <= 0) return 0;

    File.seekp(HeaderSize, std::ios::beg);
    File.seekg(HeaderSize, std::ios::beg);
    return File.good() ? 1 : 0;
}

/**
 * @brief Creates a new file for buffered I/O.
 * @param filename Name of the file to create.
 * @param mode File open mode (should include ios::out).
 * @return TRUE if file is successfully created, FALSE otherwise.
 * @pre mode includes ios::out.
 * @post File is created and header is written.
 */
int BufferFile::Create(const char* filename, std::ios::openmode mode)
{
    if (!(mode & std::ios::out)) return 0;
    // open for output; do not use nonstandard flags like ios::noreplace
    File.open(filename, mode | std::ios::out | std::ios::binary);
    if (!File.good()) { File.close(); return 0; }
    HeaderSize = WriteHeader();
    return HeaderSize != 0;
}

/**
 * @brief Closes the file if open.
 * @return TRUE if file was successfully closed or was not open, FALSE otherwise.
 * @post File is closed.
 */
int BufferFile::Close()
{
    File.close();
    return 1;
}

/**
 * @brief Rewinds the file to the position after the header.
 * @return TRUE if successful, FALSE otherwise.
 * @pre File is open.
 * @post File read and write pointers are set to just after the header.
 */
int BufferFile::Rewind() 
{ 
    File.seekg(HeaderSize, ios::beg); 
    File.seekp(HeaderSize, ios::beg); 
    return 1; 
}

/**
 * @brief Reads a record from the file into the buffer.
 * @param recaddr Record address to read from, or -1 for next record.
 * @return Address of the record read, or -1 if read failed.
 * @pre File is open and buffer is initialized.
 * @post Buffer contains the data of the record read.
 */
int BufferFile::Read(int recaddr) 
{ 
    if(recaddr ==-1) 
        return Buffer.Read(File); 
    else 
        return Buffer.DRead(File, recaddr);
}

/**
 * @brief Writes the buffer contents to the file.
 * @param recaddr Record address to write to, or -1 to append.
 * @return Address of the record written, or -1 if write failed.
 * @pre File is open and buffer is initialized with data.
 * @post Buffer contents are written to the file at the specified address or appended.
 */
int BufferFile::Write(int recaddr) 
{ 
    if(recaddr ==-1) 
        return Buffer.Write(File); 
    else 
        return Buffer.DWrite(File, recaddr);
}

/**
 * @brief Appends the buffer contents to the end of the file.
 * @return Address of the record appended, or -1 if append failed.
 * @pre File is open and buffer is initialized with data.
 * @post Buffer contents are appended to the end of the file.
 */
int BufferFile::Append() 
{ 
    File.seekp(0, ios::end); 
    return Buffer.Write(File); 
}

/**
 * @brief Returns a reference to the associated IOBuffer.
 * @return Reference to the IOBuffer used by this BufferFile.
 */
IOBuffer & BufferFile::GetBuffer() 
{
    return Buffer; 
}

/**
 * @brief Reads the buffer header from the file.
 * @return Size of the header read, or -1 if error.
 * @pre File is open and positioned at the start.
 * @post Buffer header is read from the file.
 */
int BufferFile::ReadHeader() 
{ 
    return Buffer.ReadHeader(File); 
}

/**
 * @brief Writes the buffer header to the file.
 * @return Size of the header written, or -1 if error.
 * @pre File is open and positioned at the start.
 * @post Buffer header is written to the file.
 */
int BufferFile::WriteHeader()
{ 
    return Buffer.WriteHeader(File);
}