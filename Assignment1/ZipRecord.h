#ifndef ZIPRECORD_H 
#define ZIPRECORD_H

#include <iostream>
using namespace std;



struct ZipRecord 
{
    string zip;
    string state; 
    string place; 
    string county;
    double Latitude = 0.0;
    double Longitude = 0.0;
    bool valid = false; // whether this record has valid lat/lon
};

#endif // ZIPRECORD_H