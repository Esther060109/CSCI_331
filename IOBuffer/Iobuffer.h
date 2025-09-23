#pragma once

#include <cstdlib>
#include <cstring>
#include <iostream>

// Minimal IOBuffer base class
class IOBuffer {
public:
    IOBuffer(int maxBytes = 10000);
    virtual ~IOBuffer();
    IOBuffer &operator=(const IOBuffer &);

    virtual void Clear();
    virtual int Pack(const void *field, int size = -1);
    virtual int Unpack(void *field, int maxBytes = -1);
    virtual void Print(std::ostream &) const;
    virtual int Init(int maxBytes = 10000);

    // sequential read/write (must be implemented by derived classes)
    virtual int Read(std::istream &) = 0;
    virtual int Write(std::ostream &) const = 0;

    // direct access read/write - base provides simple wrappers that can be overridden
    virtual int DRead(std::istream &in, int recref);
    virtual int DWrite(std::ostream &out, int recref) const;

    virtual int ReadHeader(std::istream &);
    virtual int WriteHeader(std::ostream &) const;

    int GetUsed() const { return BufferSize; }

protected:
    int Initialized;   // non-zero if initialized
    char *Buffer;      // underlying memory
    int MaxBytes;      // capacity
    int NextByte;      // index of next byte
    int BufferSize;    // used bytes
    int Packing;       // non-zero when packing
};
