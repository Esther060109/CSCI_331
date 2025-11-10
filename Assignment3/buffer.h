#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

typedef struct 
{ 
    unsigned int zip; 
    unsigned int length;
    string place_name; 
    string state;
    string county; 
    double latitude; 
    double longitude; 
    string tempString = ",";
    char padding[1]; 
} buffer;

void parsing(int argc, char** argv, buffer* pointer, string file, ofstream& txtFile);
void createFiles(int argc, char** argv, buffer* pointer, string file);
void sortingZip(vector<buffer>& records);
void sortingLocation(vector<buffer>& records);

void readLengthIndicatedFile(string filename, vector<buffer>& records);
bool unpackRecord(string line, buffer& record);

void generateStateTable(vector<buffer>& records);
void printStateTable();

bool readRecordAtOffset(const std::string& filename, std::streampos offset, buffer& outRecord);
void writeHeaderRecord(std::ofstream& outStream, const std::string& headerText);
bool readHeaderLine(std::ifstream& inStream, std::string& outHeader);

#endif
