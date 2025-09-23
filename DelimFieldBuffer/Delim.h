#ifndef DELIM_H
#define DELIM_H

#include<iostream>
using namespace std;

// VariableLengthBuffer is in the VariableLengthBuffer folder and the header is Varlen.h
#include "../VariableLengthBuffer/Varlen.h"

/**
 * @class DelimFieldBuffer
 * @brief Extends VariableLengthBuffer for delimiter-separated fields.
 * @details Provides methods to pack/unpack fields using a specified delimiter,
 *         read/write headers, and print buffer contents.  
 */
class DelimFieldBuffer : public VariableLengthBuffer 
{ 
    public: 
        DelimFieldBuffer(char delim = ',', int maxBytes = 10000);//constructor
       
        DelimFieldBuffer(const DelimFieldBuffer & buffer);//copy constructor 

        virtual ~DelimFieldBuffer() {} //Destructor
       
        virtual void Clear(); //clear the buffer.
        virtual int Pack(const void *field, int size = -1); //set the value of the next field of the buffer.
        virtual int Unpack(void * field, int maxBytes = -1); //get the value of the next field of the buffer.
        virtual void Print(ostream &) const; //print the contents of the buffer.
        virtual int Init(char delim = ','); //initialize the buffer with a specific delimiter.

        int ReadHeader(istream & stream); 
        int WriteHeader(ostream & stream) const; 

        static void SetDefaultDelim(char delim); 

    protected: 
        char Delim; 
        static char DefaultDelim; 
        int Initialized = 0;


};

inline DelimFieldBuffer::DelimFieldBuffer(const DelimFieldBuffer & buffer) 
:VariableLengthBuffer(buffer)
{
    Init(buffer.Delim); 
}

#endif // DELIM_H