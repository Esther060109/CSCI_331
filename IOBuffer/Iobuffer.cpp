// IOBuffer implementation - single clean translation unit
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "Iobuffer.h"

/**
 * @brief Constructor.
 * @param maxBytes Maximum number of bytes for this buffer (default 10000).
 * @post Buffer is initialized and cleared.
 */
IOBuffer::IOBuffer(int maxBytes) : Initialized(0), Buffer(nullptr), MaxBytes(0), NextByte(0), BufferSize(0), Packing(1) {
    Init(maxBytes);
}
/**
 * @brief Destructor.
 * @post Buffer is deallocated.
 */
IOBuffer::~IOBuffer() {
    delete[] Buffer;
}

/**
 * @brief Assignment operator.
 * @param buffer Another IOBuffer to copy from. 
 * @return Reference to this IOBuffer.
 * @post This IOBuffer contains a copy of the data from the source buffer.
 */
IOBuffer &IOBuffer::operator=(const IOBuffer &buffer) {
    if (this == &buffer) return *this;
    if (MaxBytes < buffer.BufferSize) {
        delete[] Buffer;
        Buffer = new char[buffer.MaxBytes];
        MaxBytes = buffer.MaxBytes;
    }
    Initialized = buffer.Initialized;
    BufferSize = buffer.BufferSize;
    if (BufferSize > 0) memcpy(Buffer, buffer.Buffer, BufferSize);
    NextByte = buffer.NextByte;
    Packing = buffer.Packing;
    return *this;
}

/**
 * @brief Clears the buffer.
 * @post Buffer is empty.
 */
void IOBuffer::Clear() {
    NextByte = 0;
    BufferSize = 0;
    Packing = 1;
}

/**
 * @brief Packs a field into the buffer.
 * @param field Pointer to the field data.
 * @param size Size of the field in bytes, or -1 if null-terminated string.
 * @return Number of bytes packed, or -1 if error.
 * @post Field data is copied into the buffer at the current position.
 */
int IOBuffer::Pack(const void *field, int size) {
    if (!field) return -1;
    const char *f = static_cast<const char *>(field);
    int n = size;
    if (size == -1) n = static_cast<int>(strlen(f)) + 1;
    if (NextByte + n > MaxBytes) return -1;
    memcpy(Buffer + NextByte, f, n);
    NextByte += n;
    BufferSize = NextByte;
    return n;
}

/**
 * @brief Unpacks a field from the buffer.
 * @param field Pointer to store the unpacked field data.
 * @param maxBytes Maximum size of the field buffer, or -1 if unknown.
 * @return Number of bytes unpacked, or -1 if error.
 * @post Field data is copied from the buffer to the provided pointer.
 */
int IOBuffer::Unpack(void *field, int maxBytes) {
    if (!field) return -1;
    if (NextByte >= BufferSize) return -1;
    char *dst = static_cast<char *>(field);
    if (maxBytes == -1) {
        int i = 0;
        while (NextByte + i < BufferSize) {
            dst[i] = Buffer[NextByte + i];
            if (dst[i] == '\0') { NextByte += i + 1; return i + 1; }
            ++i;
        }
        return -1;
    } else {
        int i = 0;
        while (i < maxBytes - 1 && NextByte + i < BufferSize) {
            dst[i] = Buffer[NextByte + i];
            if (dst[i] == '\0') { NextByte += i + 1; return i + 1; }
            ++i;
        }
        dst[i] = '\0';
        NextByte += i + 1;
        return i + 1;
    }
}

/**
 * @brief Prints buffer metadata.
 * @param stream Output stream to print to.
 */
void IOBuffer::Print(std::ostream &stream) const {
    stream << "MaxBytes=" << MaxBytes << " BufferSize=" << BufferSize << " NextByte=" << NextByte;
}

/**
 * @brief Initializes the buffer with a specified maximum size.
 * @param maxBytes Maximum size of the buffer.
 * @post Buffer is allocated and cleared.
 * @return 1 if successful, 0 otherwise.
 */
int IOBuffer::Init(int maxBytes) {
    if (maxBytes <= 0) maxBytes = 1;
    delete[] Buffer;
    MaxBytes = maxBytes;
    Buffer = new char[MaxBytes];
    BufferSize = 0;
    NextByte = 0;
    Initialized = 1;
    Packing = 1;
    return 1;
}

/**
 * @brief Direct read from a specific record address in the stream.
 * @param stream Input stream to read from.
 * @param recref Record address to read from.
 * @return Number of bytes read, or -1 if error.
 * @pre Stream is open and positioned correctly.
 * @post Buffer is filled with data read from the stream.
 */
int IOBuffer::DRead(std::istream &stream, int recref) {
    stream.clear();
    stream.seekg(recref, std::ios::beg);
    if (!stream.good()) return -1;
    return Read(stream);
}

/**
 * @brief Direct write to a specific record address in the stream.
 * @param stream Output stream to write to.
 * @param recref Record address to write to.
 * @return Number of bytes written, or -1 if error.
 * @pre Stream is open and positioned correctly.
 * @post Buffer contents are written to the stream at the specified position.
 */
int IOBuffer::DWrite(std::ostream &stream, int recref) const {
    stream.clear();
    stream.seekp(recref, std::ios::beg);
    if (!stream.good()) return -1;
    return Write(stream);
}

static const char *IOBUFFER_HEADER = "IOBuffer";
static const int IOHEADER_SIZE = static_cast<int>(strlen(IOBUFFER_HEADER));

/**
 * @brief Reads the buffer header from a stream.
 * @param stream Input stream to read from.
 * @return Position in stream of the record, or 0 if error.
 */
int IOBuffer::ReadHeader(std::istream &stream) {
    stream.clear();
    stream.seekg(0, std::ios::beg);
    if (!stream.good()) return -1;
    char buf[32];
    stream.read(buf, IOHEADER_SIZE);
    if (!stream.good()) return -1;
    if (strncmp(buf, IOBUFFER_HEADER, IOHEADER_SIZE) != 0) return -1;
    return static_cast<int>(stream.tellg());
}

/**
 * @brief Writes the buffer header to a stream.
 * @param stream Output stream to write to.
 * @return Stream position after writing header, or FALSE if error.
 */
int IOBuffer::WriteHeader(std::ostream &stream) const {
    stream.clear();
    stream.seekp(0, std::ios::beg);
    if (!stream.good()) return -1;
    stream.write(IOBUFFER_HEADER, IOHEADER_SIZE);
    if (!stream.good()) return -1;
    return static_cast<int>(stream.tellp());
}

