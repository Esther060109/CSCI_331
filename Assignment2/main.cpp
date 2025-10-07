
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

    //call parsing function for organized postal codes. 
    string file1 = "us_postal_codes.csv";
    txtFile.open("txtFile.txt"); //opens the file we just made
    parsing(argc, argv, &myBuffer, file1, txtFile); 
    txtFile.close(); 

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
    
    
    return 0; 
}
