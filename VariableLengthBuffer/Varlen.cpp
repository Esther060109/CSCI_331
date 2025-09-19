#include <iostream>
#include <cstring>
#include "Varlen.h"

VariableLengthBuffer::VariableLengthBuffer(int maxBytes)
    : IOBuffer(maxBytes)
{
    Init();
}

void VariableLengthBuffer::Clear()
{
    IOBuffer::Clear();
}

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

const char* headerStr = "Variable";
const int headerSize = strlen(headerStr);

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

int VariableLengthBuffer::WriteHeader(ostream &stream) const
{
    int result = IOBuffer::WriteHeader(stream);
    if (!result) return 0;
    stream.write(headerStr, headerSize);
    if (!stream.good()) return 0;
    return stream.tellp();
}

void VariableLengthBuffer::Print(ostream &stream) const
{
    IOBuffer::Print(stream);
}

int VariableLengthBuffer::Init()
{
    Clear();
    return 1; 
}