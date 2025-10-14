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

/**
 * @brief Buffer record structure for a single CSV/location entry.
 *
 * This struct holds parsed fields for a ZIP/place/state/county record
 * including numeric ZIP, variable-length place_name, state and county,
 * and geographic coordinates (latitude/longitude). The original comment
 * above is preserved.
 */


/**
 * @brief Parse the input CSV and populate buffers / output files.
 *
 * pointer is the pointer pointing to the buffer struct.
 * The & in the ofstream& is a REFERENCE (alias) to the original variable.
 * The pass-by-reference is necessary because std::ofstream objects are not copyable.
 *
 * @param argc command-line argument count
 * @param argv command-line argument vector
 * @param pointer pointer to a preallocated buffer struct (or array)
 * @param file input filename
 * @param txtFile output text file stream (opened by caller)
 */
void parsing (int argc, char** argv, buffer* pointer, string file, ofstream& txtFile); //pointer is the pointer pointing to the buffer struct. 
                                                                                        //the & in the ofstream& is a REFERENCE (alias) to the original variable.
                                                                                        //the pass by reference is necessary because the txtFile cannot be copied. 
/**
 * @brief Create any necessary output files based on parsed data.
 *
 * @param argc command-line argument count
 * @param argv command-line argument vector
 * @param pointer pointer to buffer data
 * @param file base filename or path used for creating outputs
 */
void createFiles(int argc, char** argv, buffer *pointer, string file);
/**
 * @brief Sort records by ZIP code (ascending).
 *
 * @param records vector of buffer records to sort in-place
 */
void sortingZip(vector<buffer>& records);  // Sort a vector of buffer records by ZIP code
/**
 * @brief Sort records by location (e.g., place_name/state) as needed.
 *
 * @param records vector of buffer records to sort in-place
 */
void sortingLocation(vector<buffer>& records);  // Sort a vector of buffer records by location


// ASSIGNMENT REQUIREMENT: Function to read and unpack length-indicated files
// This function reads files where each record starts with its length
/**
 * @brief Read a length-indicated file where each record begins with its length.
 *
 * Each record is expected in the form: [length],[comma-separated-data]\n
 * The function reads the file and unpacks each record into the provided
 * records vector.
 *
 * @param filename path to the length-indicated file
 * @param records output vector that will receive unpacked buffer records
 */
void readLengthIndicatedFile(string filename, vector<buffer>& records);

// ASSIGNMENT REQUIREMENT: Unpack a single length-indicated record
// Parses a line with format "[length][comma-separated-data]" into a buffer struct
/**
 * @brief Unpack a single length-indicated record line into a buffer struct.
 *
 * Parses a line containing a leading length followed by comma-separated fields
 * and fills the provided record structure.
 *
 * @param line input line containing one record
 * @param record output record struct to populate
 * @return true if parsing succeeded, false on malformed input
 */
bool unpackRecord(string line, buffer& record);  

// ASSIGNMENT REQUIREMENT: State analysis functions for alphabetical state table
// Analyzes all records to find extreme zip codes (easternmost, westernmost, etc.) for each state
/**
 * @brief Analyze records to generate an alphabetical state table of extremes.
 *
 * Finds the easternmost, westernmost, northernmost and southernmost ZIP codes
 * for each state present in the records vector and stores necessary data
 * for later printing.
 *
 * @param records vector of buffer records to analyze
 */
void generateStateTable(vector<buffer>& records);

// ASSIGNMENT REQUIREMENT: Print the required alphabetical state analysis table
// Displays state IDs with their easternmost, westernmost, northernmost, southernmost zip codes
/**
 * @brief Print the alphabetical state analysis table.
 *
 * Displays each state and its extreme ZIP codes (east/west/north/south)
 * according to the data previously computed by generateStateTable.
 */
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