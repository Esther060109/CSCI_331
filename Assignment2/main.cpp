
#include <iostream>
#include <string> //if working with string
#include <fstream> //used to read the csv file
#include <sstream> //pars the string we read in
#include <vector> //needed for std::vector

using namespace std;

#include "buffer.h"

/**
argc = the number of elements in the array. 
    - is an integer variable that stores the number of command-line arguments passed by the user including the name of the program
    - the value of argc should be non-negative. 
argv = is an array of character pointers (char *) listing all the arguments. 
*/

int main(int argc, char** argv) 
{ 
    // Create a buffer instance to pass to the parsing function
    buffer myBuffer;
    ofstream txtFile; //ofstream behaves like cout<<, creating a file for regular postal codes
    printf("=== Processing Regular CSV File ===\n");


    printf("\n=== Processing Randomized CSV File ===\n");
    
    //calling parsing function for random postal codes.
    string file2 = "us_postal_codes_randomized.csv";
    txtFile.open("txtFileRandom.txt");
    parsing(argc, argv, &myBuffer, file2, txtFile);
    txtFile.close();

    printf("\n=== Assignment Complete: Both CSV files processed ===\n");
    printf("Created output files:\n");
    printf("  - txtFile.txt\n");
    printf("  - txtFileRandom.txt\n");
    
    // REQUIREMENT: Read and unpack length-indicated files
    printf("\n=== READING LENGTH-INDICATED FILES ===\n");
    vector<buffer> unpackedRecords;
    
    // ASSIGNMENT REQUIREMENT: Read and unpack the length-indicated file we just created
    // This demonstrates the buffer class reading length-indicated format
    readLengthIndicatedFile("txtFileRandom.txt", unpackedRecords);
    
    // ASSIGNMENT REQUIREMENT: Generate alphabetical state table showing extreme ZIP codes
    // Required: "alphabetical listing of state IDs (two character), one state per row,
    // where for each row, the Easternmost, Westernmost, Northernmost, and Southernmost 
    // Zip Code in that state is listed, in that order"
    printf("\n=== GENERATING STATE ANALYSIS TABLE ===\n");
    generateStateTable(unpackedRecords);  // Analyze all records to find extremes
    printStateTable();                    // Display the required alphabetical table
    
    // Display first few unpacked records to verify length-indicated processing works
    printf("\nFirst 3 unpacked records:\n");
    for(int i = 0; i < 3 && i < unpackedRecords.size(); i++) {
        buffer* record = &unpackedRecords[i];
        printf("ZIP %u: %s, %s, %s (%.4f, %.4f)\n", 
               record->zip, record->place_name.c_str(),
               record->state.c_str(), record->county.c_str(),
               record->latitude, record->longitude);
    }
    
    return 0; 
}