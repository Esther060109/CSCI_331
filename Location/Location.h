#ifndef LOCATION_H
#define LOCATION_H

#include <iostream>
#include "IOBuffer.h"
using namespace std;

class Location
{
public:
    // fields for ZIP CSV
    char ZipCode[10];
    char PlaceName[32];
    char County[32];    
    char State[3];       
    char Latitude[16];
    char Longitude[16];

    // operations
    Location();
    static int InitBuffer(DelimFieldBuffer &);
    static int InitBuffer(LengthFieldBuffer &);
    void Clear();
    int Pack(IOBuffer &) const;
    int Unpack(IOBuffer &);
    void Print(ostream &, const char* label = 0) const;
};

#endif