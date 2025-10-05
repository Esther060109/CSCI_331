#include <iostream> 
#include <string> 
#include <fstream> //needed for reading strings as inputs and outputs
#include <sstream> 
#include <cstdio> //
#include <stdlib.h> //needed for the strtod
#include <vector> //needed for std::vector

#include "buffer.h"

using namespace std; 


void parsing (int argc, char** argv, buffer* pointer, string file)
{
    printf("hello world\n"); 
    char* end;
    // Create a vector to store all buffer records
    vector<buffer> records;

    //opens a file stream. 
    ifstream inputFile; //input file is not the name of the csv file.
                        //ifstream is the type and inputFile is the name. 
    inputFile.open(file);
    string line = ""; 
    getline(inputFile, line);
    line = "";


    while(getline(inputFile, line)) 
    { 
        //stringstream  allows us to read strings as if they were inputs and outputs.
        stringstream inputString(line);

        //using the tempString to convert the string to integer/decimal number. 
        getline(inputString, pointer->tempString,','); 
        pointer->zip = atoi(pointer->tempString.c_str());


        getline(inputString, pointer->place_name, ',');
        getline(inputString, pointer->state, ',');
        getline(inputString, pointer->county, ',');


        pointer->tempString = ','; //clearing the string.
        getline(inputString, pointer->tempString,',');
        pointer->latitude = strtod(pointer->tempString.c_str(), &end); 

        getline(inputString, pointer->tempString,',');
        pointer->longitude = strtod(pointer->tempString.c_str(), &end);



        // Create display utility and print the parsed data
        display printer;
        printer.print(pointer);

        // Add the parsed buffer to the vector
        records.push_back(*pointer);

        

        line = "";
    }

    // After parsing all records, iterate through them
    printf("\n--- All Records ---\n");
    for(auto& record : records) 
    { 
        display printer;
        printer.print(&record);
    }
    
}   

