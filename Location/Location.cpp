#include <iostream>
#include "location.h"

Location::Location() { Clear(); }

void Location::Clear()
{
    ZipCode[0] = 0; PlaceName[0] = 0; County[0] = 0;
    State[0] = 0; Latitude[0] = 0; Longitude[0] = 0;
}

int Location::Pack(IOBuffer &Buffer) const
{
    int numBytes;
    Buffer.Clear();
    if ((numBytes = Buffer.Pack(ZipCode)) == -1) return 0;
    if ((numBytes = Buffer.Pack(PlaceName)) == -1) return 0;
    if ((numBytes = Buffer.Pack(County)) == -1) return 0;
    if ((numBytes = Buffer.Pack(State)) == -1) return 0;
    if ((numBytes = Buffer.Pack(Latitude)) == -1) return 0;
    if ((numBytes = Buffer.Pack(Longitude)) == -1) return 0;
    return 1;
}

int Location::Unpack(IOBuffer &Buffer)
{
    Clear();
    int numBytes;
    if ((numBytes = Buffer.Unpack(ZipCode)) == -1) return 0;
    if ((numBytes = Buffer.Unpack(PlaceName)) == -1) return 0;
    if ((numBytes = Buffer.Unpack(County)) == -1) return 0;
    if ((numBytes = Buffer.Unpack(State)) == -1) return 0;
    if ((numBytes = Buffer.Unpack(Latitude)) == -1) return 0;
    if ((numBytes = Buffer.Unpack(Longitude)) == -1) return 0;
    return 1;
}

int Location::InitBuffer(DelimFieldBuffer &Buffer) { return 1; }
int Location::InitBuffer(LengthFieldBuffer &Buffer) { return 1; }

void Location::Print(ostream &stream, const char* label) const
{
    if (!label) stream << "Zip Code Record:";
    else stream << label;
    stream << "\n\tZipCode: " << ZipCode
           << "\n\tPlace: " << PlaceName
           << "\n\tCounty: " << County
           << "\n\tState: " << State
           << "\n\tLatitude: " << Latitude
           << "\n\tLongitude: " << Longitude
           << endl;
}