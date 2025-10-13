#ifndef BUFFER_H 
#define BUFFER_H

#include <iostream> 
#include <string> 
#include <fstream> 
#include <sstream> 
#include <vector> 
#include <map> //needed for primary key index

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



void parsing (int argc, char** argv, buffer* pointer, string file, ofstream& txtFile); //pointer is the pointer pointing to the buffer struct. 
                                                                                        //the & in the ofstream& is a REFERENCE (alias) to the original variable.
                                                                                        //the pass by reference is necessary because the txtFile cannot be copied. 
void createFiles(int argc, char** argv, buffer *pointer, string file);
void sortingZip(vector<buffer>& records);  // Sort a vector of buffer records by ZIP code
void sortingLocation(vector<buffer>& records);  // Sort a vector of buffer records by location


// ASSIGNMENT REQUIREMENT: Function to read and unpack length-indicated files
// This function reads files where each record starts with its length
void readLengthIndicatedFile(string filename, vector<buffer>& records);

// ASSIGNMENT REQUIREMENT: Unpack a single length-indicated record
// Parses a line with format "[length][comma-separated-data]" into a buffer struct
bool unpackRecord(string line, buffer& record);  

// ASSIGNMENT REQUIREMENT: State analysis functions for alphabetical state table
// Analyzes all records to find extreme zip codes (easternmost, westernmost, etc.) for each state
void generateStateTable(vector<buffer>& records);

// ASSIGNMENT REQUIREMENT: Print the required alphabetical state analysis table
// Displays state IDs with their easternmost, westernmost, northernmost, southernmost zip codes
void printStateTable();

// Lines addes :Loana

/// Read a single record from a length-indicated file at the given byte offset.
/// @param filename length-indicated file
/// @param offset byte offset where the record begins (should be the beginning of the line)
/// @param outRecord parsed buffer struct (one record only)
/// @return true on success
bool readRecordAtOffset(const std::string& filename, std::streampos offset, buffer& outRecord);

/// Write a simple header record to a length-indicated file (used when creating a file).
/// Header is written as: [header_length],[header_text]\n
/// @param outStream open ofstream to write header into (must be open & at file start)
/// @param headerText textual header describing schema
void writeHeaderRecord(std::ofstream& outStream, const std::string& headerText);

/// Read and return the header line (first line) from an already-open input file stream.
/// @param inStream input stream positioned at beginning
/// @param outHeader receives the header line (full line)
/// @return true if read
bool readHeaderLine(std::ifstream& inStream, std::string& outHeader);

#endif