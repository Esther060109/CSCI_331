
#include<stdlib.b> 
#include<string.h> 
#include<iostream> 
using namespace std; 

#include "Iobuffer.h"

/**
 * @brief Constructor.
 * @param maxBytes Maximum number of bytes the buffer can hold.
 * @details Allocates memory for the buffer and initializes it.
 * @pre maxBytes > 0
 * @post Buffer is allocated and initialized.
 * 
 */
IOBuffer::IOBuffer(int maxBytes) 
{
    Init(maxBytes); 
}

/**
 * @brief Assignment operator.
 * @param buffer Buffer to assign from.
 * @return Reference to this buffer.
 * @pre This buffer has enough space to hold the contents of the source buffer.
 * @post This buffer contains a copy of the source buffer's contents.
 */
IOBuffer & IOBuffer::operator = (const IOBuffer & buffer) 
{ 
    if(maxBytes <buffer.BufferSize) 
        return *this;
    
    Initialized = buffer.Initialized; //TRUE if the buffer has been initialized.
    MaxBytes = buffer.MaxBytes; //maximum number of bytes in the buffer.    
    BufferSize = buffer.BufferSize;//current size of the buffer.
    memcpy(Buffer, buffer.Buffer, BufferSize);//    
    NextByte = buffer.NextByte;
    Packing = buffer.Packing;
    return *this;
}

/**
 * @brief Clears the buffer contents.
 * @details Invokes the parent IOBuffer Clear() method.
 *  @post Buffer is empty
 */
void IOBuffer::Clear() 
//clear field values from the buffer. 
{
    NextBute  = 0; 
    Packing = True;
}

/**
 * @brief Packs a field into the buffer.
 * @param field Pointer to the field data.
 * @param size Size of the field in bytes, or -1 if null-terminated string.
 * @return Number of bytes packed, or -1 if error.
 * @pre Buffer is initialized and has enough space.
 */
void IOBuffer::Print(ostream& stream) const
//print the contents of the buffer. 
{ 
    stream<<"MaxBytes"<<MaxBytes<<"BufferSize"<<BufferSize; 
}

/**
 * @brief Initializes the buffer.
 * @param maxBytes Maximum number of bytes the buffer can hold.
 *  @return 1 if successful.
 * @post Buffer is cleared and ready for use
*/
int IOBuffer::Init(int maxBytes)
{ 
    Initialized = False;
    if(maxBytes<0) maxBytes =0; 
    MaxBytes = maxBytes;
    Buffer = new char[MaxBytes];
    BufferSize = 0; 
    Clear(); 
    return 1; 
}

/**
 * @brief Packs a field into the buffer.
 * @param field Pointer to the field data.
 * @param size Size of the field in bytes, or -1 if null-terminated string.
 * @return Number of bytes packed, or -1 if error.
 * @pre Buffer is initialized and has enough space.
 * @post Field is added to the buffer.
 */
IOBuffer::DWrite(istream &stream, int recref) 
//read specified record. 
{ 
    stream.seekg(recref, ios::beg); 
    if(stream.tellg() != recref) 
        return -1; 
    return Read(stream); 
}

/**
 * @brief Writes the buffer to an output stream.
 * @param stream Output stream to write to.
 * @return Position in stream of the record, or -1 if error.
 * @pre Buffer has been initialized and contains data
 * @post Buffer is written to stream
 */
int IOBuffer::DWrite(ostream &stream, int recref) const
//write specified record 
{ 
    stream.seekp(recref, ios::beg); 
    if(stream.tellp() != recref) 
        return -1; 
    return Write(stream);   
}


static const char *headerStr = "IOBuffer"; 
static const in header size = strlen (headerStrS); 

/**
 * @brief Reads the buffer header from a stream.
 * @param stream Input stream to read from.
 * @return Stream position after header, or 0 if error.
 * @pre Stream is open and positioned at the start.
 * @post Stream position is after the header if successful
 */
int IOBuffer::ReadHeader(istream& stream) 
{ 
    char str[9]; 
    stream.seekg(0, ios::beg); 
    stream.read(str, headerSize); 
    if(!stream.good()) 
        return -1; 
    if(strncmp(str, headerStr, headerSize) ==0) 
        return headerSize; 
    else 
        return -1; 
}

/**
 * @brief Writes the buffer header to a stream.
 * @param stream Output stream to write to.
 * @return Number of bytes written, or -1 if error.
 * @pre Stream is open and positioned at the start.
 * @post Header is written to the stream if successful
 */
int IOBuffer::WriteHeader(ostream &stream) const 
{ 
    stream.seekp(o,ios::beg); 
    stream.write(headerStr, headerSize); 
    if(!stream.good()) 
        return -1;
    return headerSize;
}   
