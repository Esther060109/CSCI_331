#ifndef BUFFILE_H 
#define BUFFILE_H 

#include <fstream> 
#include <iostream> 
using namespace std; 

#include "Iobuffer.h"

class BufferFile
{ 

    //Class to represent buffered file operations used in conjuction with the IOBuffer classes. 
    //Each Buffered file is associated with a disk file of a specific record type. 
    //Each buffered file object has a buffer object which can be used for file I/O
    
    public: 
        explicit BufferFile(IOBuffer &); //Constructor 
        virtual ~BufferFile(); //Destructure

        int Open(char *filename, int Mode); 
        int Create(const char * filename, int Mode); 
        int Close (); 
        int Rewind(); 
        //Input and Output operations 
        int Read(int Recaddr =-1); 
            //read a record into the buffer
            //return the record address 
            //return<0 if read failed 
            //if recaddr ==-1, read the next record in the file
            // if recaddr != -1, read the record at that address
        int Write (int recaddr =-1); 
        int Append(); 
        IOBuffer &GetBuffer(); 

    protected: 
        IOBuffer & Buffer; 
        fstream File; 
        int HeaderSize = 0; //size of header 
        
        int ReadHeader(); 
        int WriteHeader(); 

};

