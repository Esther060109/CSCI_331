#ifndef BUFFER_H 
#define BUFFER_H

#include <iostream> 
#include <string> 
#include <fstream> 
#include <sstream> 
#include <vector> 

using namespace std; 

/*
    The structure declares the variables of the zip, place name
    , state, county, lat, and long.
*/
typedef struct 
{ 
    unsigned int zip; 
    unsigned int length;
    string place_name; 
    string state;
    string county; 
    double latitude; 
    double longitude; 
    string tempString= ",";//initializing the tempString.  
    char padding[1]; 

}buffer;

struct display 
{ 
    public: 
        // No constructor needed - this is a utility class
        
        void print(buffer* pointer) 
        { 
            // printf("%u",pointer->length,", ");
            // printf("zip code: %u, ", pointer->zip); 
            // printf("Name of the place: %s, ", pointer->place_name.c_str()); 
            // printf("State: %s, ", pointer->state.c_str()); 
            // printf("County: %s, ", pointer->county.c_str()); 
            // printf("Latitude: %.4f, ", pointer->latitude);
            // printf("Longitude: %.4f\n", pointer->longitude);
        }
};

// length-indicated CSV/record handling
void parsingLengthFile(int argc, char** argv, buffer* pointer, string csvFile, ofstream& lengthFile);
bool readLengthRecord(ifstream& inFile, buffer* pointer);
unsigned int countCSVRecords(string csvFile);

// header record
typedef struct
{
    string file_type = "LengthIndicatedZip";
    int version = 1;
    unsigned int header_size;
    unsigned int record_count;
    unsigned int field_count = 6; 
    string primary_key = "zip";
    vector<string> field_names = {"zip", "place_name", "state", "county", "latitude", "longitude"};
} HeaderRecord;

void writeHeaderRecord(ofstream& out, HeaderRecord* header);
void readHeaderRecord(ifstream& in, HeaderRecord* header);

void parsing (int argc, char** argv, buffer* pointer, string file, ofstream& txtFile); //pointer is the pointer pointing to the buffer struct. 
                                                                                        //the & in the ofstream& is a REFERENCE (alias) to the original variable.
                                                                                        //the pass by reference is necessary because the txtFile cannot be copied. 
void createFiles(int argc, char** argv, buffer *pointer, string file);
void print(buffer* pointer); 


#endif