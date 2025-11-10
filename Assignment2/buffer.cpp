#include <iostream> 
#include <string> 
#include <fstream> //needed for reading strings as inputs and outputs
#include <sstream> 
#include <cstdio> //
#include <stdlib.h> //needed for the strtod
#include <vector> //needed for std::vector
#include <algorithm> //needed for sorting the structs
#include <map> //needed for primary key index
using namespace std; 

#include "buffer.h"



// ASSIGNMENT REQUIREMENT: Core parsing function for CSV to length-indicated format conversion
// This function implements the buffer class requirement to read ZIP code CSV files
// and convert them to length-indicated file structure format
void parsing (int argc, char** argv, buffer* pointer, string file, ofstream& txtFile)
{
    //printf("hello world\n"); //test line
    char* end;

    // ASSIGNMENT REQUIREMENT: Write header record FIRST for length-indicated format
    // "beginning with a header record where fields are comma separated but records are length indicated"
    string header = "zip,place_name,state,county,latitude,longitude";
    txtFile << header.length() << "," << header << endl;

    // Create a vector to store all buffer records for processing
    vector<buffer> records; //a vector is a container. buffer is the type, records is the name.

    //opens a file stream for reading the CSV input file
    ifstream inputFile; //input file is not the name of the csv file.
                        //ifstream is the type and inputFile is the name. 
    inputFile.open(file);
    string line = ""; 
    
    // ASSIGNMENT REQUIREMENT: Skip the CSV header lines (multi-line header in the source CSV)
    // This ensures we only process actual ZIP code data records
    getline(inputFile, line); // Skip "Zip
    getline(inputFile, line); // Skip Code","Place  
    getline(inputFile, line); // Skip Name",State,County,Lat,Long

    // ASSIGNMENT REQUIREMENT: Process each CSV record line
    while(getline(inputFile, line)) 
    { 
        //stringstream allows us to read strings as if they were inputs and outputs.
        stringstream inputString(line);

        //get the length of the line for length-indicated format requirement
        pointer->length = line.length();
        
        // Parse ZIP code field (convert string to integer)
        getline(inputString, pointer->tempString,','); 
        pointer->zip = atoi(pointer->tempString.c_str());

        // Parse place name, state abbreviation, and county name fields
        getline(inputString, pointer->place_name, ',');
        getline(inputString, pointer->state, ',');
        getline(inputString, pointer->county, ',');

        // Parse latitude coordinate (convert string to double)
        pointer->tempString = ','; //clearing the string.
        getline(inputString, pointer->tempString,',');
        pointer->latitude = strtod(pointer->tempString.c_str(), &end); 

        // Parse longitude coordinate (convert string to double)
        getline(inputString, pointer->tempString,',');
        pointer->longitude = strtod(pointer->tempString.c_str(), &end);

        // ASSIGNMENT REQUIREMENT: Add the parsed buffer to the vector for processing
        //push_back adds the current record to the vector collection
        records.push_back(*pointer);

        // DON'T write to file yet - wait until after sorting per assignment requirements
        // txtFile << pointer->length<< "," << pointer->zip << "," << pointer->place_name << "," << pointer->state << "," << pointer->county << "," << pointer->latitude << "," << pointer->longitude << endl;

        line = ""; // Clear line for next iteration
    }

    // ASSIGNMENT REQUIREMENT: Sort the records before writing to demonstrate sorting functionality
    sortingZip(records);      // Sort by ZIP code
    sortingLocation(records); // Sort by geographic location
    
    // ASSIGNMENT REQUIREMENT: Write all records to length-indicated format file
    // Each record format: [length],[zip],[place],[state],[county],[latitude],[longitude]
    for (const auto& record : records) 
    {
        txtFile << record.length << "," << record.zip << "," << record.place_name << "," << record.state << "," << record.county << "," << record.latitude << "," << record.longitude << endl;
    }

    // Close input file and complete parsing process
    inputFile.close();
}

// ASSIGNMENT REQUIREMENT: Sort function - sorts buffer records by zip code  
// This function implements sorting capability for the buffer class
void sortingZip(vector<buffer>& records) 
{
    printf("Sorting %lu records by ZIP code...\n", records.size());
    
    // Using built-in sort with lambda function for ZIP code ascending order
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.zip < b.zip;  // Sort by ZIP code ascending
    });
    
    printf("Sorting complete!\n");
}

// ASSIGNMENT REQUIREMENT: Sort function - sorts buffer records by geographic location
// This provides alternative sorting by latitude for geographic analysis
void sortingLocation(vector<buffer>& records) 
{
    printf("Sorting %lu records by location...\n", records.size());

    // Using built-in sort with lambda function for latitude ascending order
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.latitude < b.latitude;  // Sort by latitude ascending
    });
    
    printf("Sorting complete!\n");
}

// ASSIGNMENT REQUIREMENT: Read and unpack length-indicated Zip Code files
// This function implements the requirement to modify buffer class to read length-indicated format
void readLengthIndicatedFile(string filename, vector<buffer>& records) {
    printf("Reading length-indicated file: %s\n", filename.c_str());
    
    ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        printf("Error: Could not open length-indicated file %s\n", filename.c_str());
        return;
    }
    
    records.clear();  // Clear any existing data
    string line;
    int recordCount = 0;
    
    // Skip header record (first line contains metadata about the file structure)
    getline(inputFile, line);
    printf("Header: %s\n", line.c_str());
    
    // ASSIGNMENT REQUIREMENT: Read and unpack each length-indicated record
    // Each record format: [length],[zip],[place],[state],[county],[latitude],[longitude]
    while (getline(inputFile, line)) {
        buffer record;
        if (unpackRecord(line, record)) {  // Parse the length-indicated format
            records.push_back(record);
            recordCount++;
        }
    }
    
    inputFile.close();
    printf("Successfully read and unpacked %d records from length-indicated file.\n", recordCount);
}

// ASSIGNMENT REQUIREMENT: Unpack a single length-indicated record
// Format example: "42,501,Holtsville,NY,Suffolk,40.8154,-73.0451"
// This implements the requirement to read and unpack records from length-indicated files
bool unpackRecord(string line, buffer& record) {
    if (line.empty()) return false;
    
    stringstream ss(line);
    string lengthStr, zipStr, latStr, lonStr;
    
    // Parse format: length,zip,place_name,state,county,latitude,longitude
    getline(ss, lengthStr, ',');  // Get the declared record length
    int expectedLength = atoi(lengthStr.c_str());
    
    // Get the actual data part (everything after first comma)
    string dataStr = line.substr(lengthStr.length() + 1);
    
    // Validate that actual length matches declared length (assignment requirement)
    if (dataStr.length() != expectedLength) {
        printf("Warning: Length mismatch in record. Expected %d, got %lu\n", 
               expectedLength, dataStr.length());
    }
    
    // Parse the comma-separated data fields: zip,place_name,state,county,latitude,longitude
    stringstream dataStream(dataStr);
    
    // Extract ZIP code (convert string to integer)
    getline(dataStream, zipStr, ',');
    record.zip = atoi(zipStr.c_str());
    
    // Extract place name, state abbreviation, and county name (strings)
    getline(dataStream, record.place_name, ',');
    getline(dataStream, record.state, ',');
    getline(dataStream, record.county, ',');
    
    // Extract latitude coordinate (convert string to double)
    getline(dataStream, latStr, ',');
    record.latitude = strtod(latStr.c_str(), nullptr);
    
    // Extract longitude coordinate (convert string to double) 
    getline(dataStream, lonStr);
    record.longitude = strtod(lonStr.c_str(), nullptr);
    
    // Store the actual data length for validation purposes
    record.length = dataStr.length();
    
    return true;
}

// ASSIGNMENT REQUIREMENT: State analysis functionality
// This struct holds the extreme zip codes for each state as required by the assignment
struct StateExtremotes {
    string state;                      // State abbreviation (2 characters)
    buffer easternmost;                // ZIP with least longitude (furthest east)
    buffer westernmost;                // ZIP with greatest longitude (furthest west)
    buffer northernmost;               // ZIP with greatest latitude (furthest north)
    buffer southernmost;               // ZIP with least latitude (furthest south)
    bool initialized = false;          // Flag to track if this state has been processed
};

// Global map to store state extreme data for alphabetical processing
// Key: state abbreviation, Value: StateExtremotes struct
map<string, StateExtremotes> stateData;

// ASSIGNMENT REQUIREMENT: Generate state analysis table
// This function analyzes all records to find extreme ZIP codes for each state
// Required output: alphabetical listing with easternmost, westernmost, northernmost, southernmost ZIPs
void generateStateTable(vector<buffer>& records) {
    printf("Generating state analysis table...\n");
    stateData.clear();  // Clear any previous data
    
    // Process each ZIP code record to find state extremes
    for(auto& record : records) {
        string state = record.state;
        if(state.empty()) continue;  // Skip records without state data
        
        if(stateData.find(state) == stateData.end()) {
            // First ZIP code encountered for this state - initialize all extremes to this record
            StateExtremotes extremes;
            extremes.state = state;
            // Initially, this record represents all four extremes
            extremes.easternmost = extremes.westernmost = 
            extremes.northernmost = extremes.southernmost = record;
            extremes.initialized = true;
            stateData[state] = extremes;
        } else {
            // Update extremes for this state by comparing coordinates
            StateExtremotes& extremes = stateData[state];
            
            // EASTERNMOST: ZIP with smallest longitude (furthest east)
            if(record.longitude < extremes.easternmost.longitude) {
                extremes.easternmost = record;
            }
            // WESTERNMOST: ZIP with largest longitude (furthest west)  
            if(record.longitude > extremes.westernmost.longitude) {
                extremes.westernmost = record;
            }
            // NORTHERNMOST: ZIP with largest latitude (furthest north)
            if(record.latitude > extremes.northernmost.latitude) {
                extremes.northernmost = record;
            }
            // SOUTHERNMOST: ZIP with smallest latitude (furthest south)
            if(record.latitude < extremes.southernmost.latitude) {
                extremes.southernmost = record;
            }
        }
    }
    
    printf("State analysis complete for %lu states.\n", stateData.size());
}

// ASSIGNMENT REQUIREMENT: Print the required alphabetical state analysis table
// Output format: State ID (2 char) | Easternmost ZIP | Westernmost ZIP | Northernmost ZIP | Southernmost ZIP
// Table must be alphabetical by state ID and include appropriate headers
void printStateTable() {
    printf("\n=== ALPHABETICAL STATE ANALYSIS TABLE ===\n");
    
    // Print header row with column labels as required
    printf("%-5s %-15s %-15s %-15s %-15s\n", 
           "State", "Easternmost", "Westernmost", "Northernmost", "Southernmost");
    printf("%-5s %-15s %-15s %-15s %-15s\n", 
           "-----", "---------------", "---------------", "---------------", "---------------");
    
    // Iterate through map (automatically alphabetical due to map's sorted nature)
    // Each row shows: state abbreviation and its four extreme ZIP codes
    for(auto& pair : stateData) {
        StateExtremotes& extremes = pair.second;
        printf("%-5s %-15u %-15u %-15u %-15u\n",
               extremes.state.c_str(),          // 2-character state ID
               extremes.easternmost.zip,        // ZIP with least longitude
               extremes.westernmost.zip,        // ZIP with greatest longitude  
               extremes.northernmost.zip,       // ZIP with greatest latitude
               extremes.southernmost.zip);      // ZIP with least latitude
    }
}

//lines added :Loana
/**
 * Read a single length-indicated record from filename using the file offset from index.
 */
bool readRecordAtOffset(const std::string& filename, std::streampos offset, buffer& outRecord) {
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Error: cannot open file " << filename << '\n';
        return false;
    }
    in.clear();
    in.seekg(offset);
    if (!in.good()) {
        std::cerr << "Error: seek failed to offset " << static_cast<long long>(offset) << '\n';
        in.close();
        return false;
    }
    std::string line;
    if (!std::getline(in, line)) {
        std::cerr << "Error: cannot read record at offset " << static_cast<long long>(offset) << '\n';
        in.close();
        return false;
    }
    in.close();
    return unpackRecord(line, outRecord);
}

/**
 * Write a header record into a length-indicated file.
 */
void writeHeaderRecord(std::ofstream& outStream, const std::string& headerText) {
    if (!outStream.is_open()) return;
    std::string header = headerText;
    // Header line format: [length],[header-text]\n
    outStream << header.length() << "," << header << "\n";
}

/**
 * Read header line from an input file stream (first line)
 */
bool readHeaderLine(std::ifstream& inStream, std::string& outHeader) {
    if (!inStream.is_open()) return false;
    inStream.clear();
    inStream.seekg(0);
    if (!std::getline(inStream, outHeader)) return false;
    return true;
}