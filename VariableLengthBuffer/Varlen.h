#ifndef VARLEN_H
#define VARLEN_H

#include <iostream>
#include <cstring>   
#include "IOBuffer.h"

using namespace std;

class VariableLengthBuffer : public IOBuffer
// Abstract class designed to support variable-length records
// Fields may be of a variety of types
{
public:
    VariableLengthBuffer(int MaxBytes = 1000);
    
    VariableLengthBuffer(const VariableLengthBuffer &buffer)
        // copy constructor
        : IOBuffer(buffer) {}

    void Clear();                  // clear fields from buffer
    int Read(istream &);           // read a buffer from stream
    int Write(ostream &) const;    // write a buffer to stream
    int ReadHeader(istream &);     // read the header from stream
    int WriteHeader(ostream &) const; // write header to stream

    // packing methods for CSV fields
    int PackFixLen(void *, int);
    int PackDelimeted(void *, int);
    int PackLength(void *, int);

    void Print(ostream &) const;
    int SizeOfBuffer() const;      // return current size of buffer
    int Init();
};

#endif
