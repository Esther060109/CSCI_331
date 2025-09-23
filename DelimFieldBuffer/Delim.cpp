#include "Delim.h" 
#include"Iobuffer.h" 
#include<iostream> 
using namespace std;

#define TRUE 1 
#define FALSE 0

/**
 * @brief Constructor.
 * @param delim Delimiter character (default comma).
 * @param maxBytes Maximum number of bytes for the buffer (default 10000).
 * @post Buffer is initialized with the specified delimiter and cleared.
 */
DelimFieldBuffer::DelimFieldBuffer(char delim, int maxBytes)
:VariableLengthBuffer(maxBytes)
{
    Init(delim); 
}

/**
 * @brief Destructor.
 * @post Buffer resources are released.
 */
void DelimFieldBuffer::Clear() 
{ 
    VariableLengthBuffer::Clear();
}

/**
 * @brief Packs a field into the buffer using the delimiter.
 * @param field Pointer to the field data.
 * @param size Size of the field in bytes, or -1 if null-terminated string.
 * @return Number of bytes packed, or -1 if error.
 * @pre Buffer is initialized and has enough space.
 * @post Field is added to the buffer followed by the delimiter.
 */
int DelimFieldBuffer:: Pack(const void *field, int size) 
{ 
    short len; 
    if(size >= 0) len = size; 
    else len = strlen((char*)field); 
    if(len>(short)strlen((char*)field)) 
        return -1; 
    int start = NextByte; 
    NextByte += len +1; 
    if(NextByte > MaxBytes) 
        return -1;
    memcpy(&Buffer[start], field, len);
    Buffer[start+len] = Delim;
    BufferSize = NextByte; 
    return len;
}

/**
 * @brief Unpacks a field from the buffer using the delimiter.
 * @param field Pointer to store the unpacked field data.
 * @param maxBytes Maximum size of the field buffer, or -1 if unknown.
 * @return Number of bytes unpacked, or -1 if error.
 * @pre Buffer is initialized and contains data.
 * @post Field is extracted from the buffer up to the delimiter.
 *       The field is null-terminated if space allows.
 */
int DelimFieldBuffer::ReadHeader(istream &stream)
{
    char ch; 
    int result; 
    result = VariableLengthBuffer::ReadHeader (stream); 
    if(!result) 
        return FALSE; 
    stream.get(ch); 
    if(!Initialized) 
    { 
        SetDefaultDelim(ch); 
        return TRUE;    
    }
    if(ch !=Delim)
        return FALSE;
    return stream.tellg();      
}

/**
 * @brief Writes the buffer header to a stream, including the delimiter.
 * @param stream Output stream to write the header.
 * @return Stream position after writing header, or FALSE if error.
 * @pre Buffer is initialized.
 * @post Header and delimiter are written to the stream.
 */
int DelimFieldBuffer::WriteHeader(ostream & stream) const 
{ 
    if(!Initialized)    
        return FALSE; 
    int result = VariableLengthBuffer::WriteHeader(stream); 
    if(!result) 
        return FALSE; 
    stream.put(Delim); 
    return stream.tellp(); 
}

/**
 * @brief Prints the contents of the buffer, including the delimiter.
 * @param stream Output stream to print to.
 * @post Buffer contents and delimiter are written to the stream.
 */
void DelimFieldBuffer::Print(ostream &stream) const 
{ 
    VariableLengthBuffer::Print(stream); 
    stream<<"Delimiter'"<<Delim<<"'"<<endl; 

}

/**
 * @brief Initializes the buffer with a specific delimiter.
 * @param delim Delimiter character to use.
 * @return TRUE if successful, FALSE otherwise.
 * @post Buffer is cleared and set to use the specified delimiter.
 */
int DelimFieldBuffer::Init(char delim) 
{ 
    Initialized = TRUE; 
    Clear(); 
    if(delim == -1) 
        Delim = DefaultDelim; 
    else 
        Delim = delim; 
    return TRUE; 
}

/**
 * @brief Sets the default delimiter character for all DelimFieldBuffer instances.
 * @param delim Delimiter character to set as default.
 * @post DefaultDelim is updated to the specified character.    
 */
void DelimFieldBuffer:: SetDefaultDelim (char delim) 
{ 
    DefaultDelim = delim; 
}
char DelimFieldBuffer :: DefaultDelim = 0; 

int DelimFieldBuffer::Unpack(void *field, int maxBytes) 
{ 
    int start = NextByte; 
    int i = start;
    while(i < BufferSize && Buffer[i] != Delim)
        i++;
    if(i == BufferSize) 
        return -1; 
        
    int len =i-start; 
    if(maxBytes == -1 || len<maxBytes) 
        ((char*)field)[len] = '\0';

    NextByte =i+1; 
    return len;
}