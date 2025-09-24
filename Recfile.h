#pragma once
#include "../BufferFile/Buffile.h"

#include <iostream> 
using namespace std; 
#include <vector>


/**
 * @brief Template class for record-based file operations.
 * @details Provides methods to read, write, and append records of type RecType
 *          using an associated IOBuffer for buffering.
 */
template<class RecType> 
/**
 * @brief RecordFile class for handling records of type RecType.
 * @details Inherits from BufferFile and provides methods to read, write,
 *         and append records using the associated IOBuffer.    
 */
class RecordFile: public BufferFile 
{ 
    public: 
        RecordFile(IOBuffer & buffer): BufferFile(buffer){} 
        int Read(RecType & record, int recaddr = -1); 
        int Write(const RecType & record, int recaddr =-1); 
        int Append(const RecType & record, int recaddr =-1); 
       
};


template<class RecType> 
/**
 * @brief Reads a record from the file into the provided record object.
 * @param record Reference to a RecType object to populate with the read data.
 * @param recaddr Record address to read from, or -1 for next record.   
 */
int RecordFile<RecType>::Read(RecType & record, int recaddr = -1) 
{ 
    int readAddr, result; 
    readAddr = BufferFile::Read (recaddr); 
    if(readAddr == -1) 
        return -1; 
    result = record.Unpack(Buffer);
    if(!result) 
        result -1; 
    return readAddr; 
}

template<class RecType> 
/**
 * @brief Writes the provided record to the file.
 * @param record Reference to a RecType object containing the data to write.
 * @param recaddr Record address to write to, or -1 to append.
 * @return Address of the record written, or -1 if write failed.
 * @pre Buffer is initialized with data from record.
 * @post Record data is written to the file at the specified address or appended.
 */
int RecordFile<RecType>::Write(const RecType& record, int recaddr =-1)
{
    int result; 
    result = record.Pack(Buffer); 
    if(!result) 
        result = -1; 
    return BufferFile::Write(recaddr); 
}

template<class RecType> 
/**
 * @brief Appends the provided record to the end of the file.
 * @param record Reference to a RecType object containing the data to append.
 * @param recaddr Optional record address (default -1, ignored for append).
 * @return Address of the record appended, or -1 if append failed.
 * @pre Buffer is initialized with data from record.
 * @post Record data is appended to the end of the file.
 */
int RecordFile<RecType>::Append(const RecType& record, int recaddr =-1) 
{ 
    int result; 
    result = record.Pack(Buffer); 
    if(!result) 
        return -1; 
    return BufferFile::Append();
}
