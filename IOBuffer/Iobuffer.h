#ifndef IOBUFFER_H 
#define IOBUFFER_H 

#include <iostream>
using namespace std;
/**
 * @brief Constructor.
 * @param maxBytes Maximum number of bytes the buffer can hold.
 * @details Allocates memory for the buffer and initializes it.
 * @pre maxBytes > 0
 * @post Buffer is allocated and initialized.
 * 
 */

class IOBuffer 
{ 
    public: 
        IOBuffer(int maxBytes = 10000);//constructor
        
        virtual ~IOBuffer();//destructor 
        
        IOBuffer & operator = (const IOBuffer & );//assignment operator
        
        virtual void Clear(); //clear the buffer.
        virtual int Pack(const void *field, int size = -1); //set the value of the next field of the buffer.
        virtual int Unpack(void * field, int maxBytes = -1); //get the value of the next field of the buffer.
        virtual void Print(ostream &) const; //print the contents of the buffer.
        virtual int Init(); //initialize the buffer.
        
        //read and write methods return the address of the record
        //sequential read and write operations. 
        virtual int Read(istream &) = 0;
        //read a buffer from a stream.
        virtual int Write(ostream &) const = 0;
        //write a buffer to a stream.

        //direct access read and write operations
        virtual int DRead(istream &, int recref) = 0;
        //read specific record. 
        virtual int DWrite(ostream &, int recref) const = 0;
        //write specific record.    
        virtual int ReadHeader(istream &); //read the buffer header from a stream.
        virtual int WriteHeader(ostream &) const; //write the buffer header to a stream.    

        protected: 
            int Initialized = 0; //TRUE if the buffer has been initialized.
            char *Buffer = nullptr; //character array to hold field values.
            int MaxBytes = 0; //maximum number of bytes in the buffer.
            int NextByte = 0; //index of next byte to be packed or unpacked.
            int BufferSize = 0; //current size of the buffer.
            int Packing = 0; //TRUE if packing, FALSE if unpacking.
};
