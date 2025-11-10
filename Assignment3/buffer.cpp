#include "buffer.h"
#include <cstdlib>
#include <algorithm>

using namespace std;

void parsing(int argc, char** argv, buffer* pointer, string file, ofstream& txtFile)
{
    string header = "zip,place_name,state,county,latitude,longitude";
    txtFile << header.length() << "," << header << endl;

    vector<buffer> records;

    ifstream inputFile(file);
    string line;

    getline(inputFile, line); // skip headers
    getline(inputFile, line);
    getline(inputFile, line);

    while(getline(inputFile, line))
    {
        stringstream inputString(line);

        pointer->length = line.length();
        getline(inputString, pointer->tempString, ',');
        pointer->zip = atoi(pointer->tempString.c_str());

        getline(inputString, pointer->place_name, ',');
        getline(inputString, pointer->state, ',');
        getline(inputString, pointer->county, ',');

        getline(inputString, pointer->tempString, ',');
        pointer->latitude = strtod(pointer->tempString.c_str(), nullptr);

        getline(inputString, pointer->tempString, ',');
        pointer->longitude = strtod(pointer->tempString.c_str(), nullptr);

        records.push_back(*pointer);
        line = "";
    }

    inputFile.close();

    sortingZip(records);
    sortingLocation(records);

    for (const auto& record : records)
    {
        txtFile << record.length << "," << record.zip << "," << record.place_name << ","
                << record.state << "," << record.county << "," << record.latitude << "," << record.longitude << endl;
    }
}

void sortingZip(vector<buffer>& records)
{
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.zip < b.zip;
    });
}

void sortingLocation(vector<buffer>& records)
{
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.latitude < b.latitude;
    });
}

void readLengthIndicatedFile(string filename, vector<buffer>& records)
{
    ifstream inputFile(filename);
    if (!inputFile.is_open()) return;

    records.clear();
    string line;
    getline(inputFile, line); // skip header

    while(getline(inputFile, line))
    {
        buffer record;
        if (unpackRecord(line, record))
            records.push_back(record);
    }

    inputFile.close();
}

bool unpackRecord(string line, buffer& record)
{
    if (line.empty()) return false;

    stringstream ss(line);
    string lengthStr, zipStr, latStr, lonStr;

    getline(ss, lengthStr, ',');
    //int expectedLength = atoi(lengthStr.c_str());

    string dataStr = line.substr(lengthStr.length() + 1);

    stringstream dataStream(dataStr);
    getline(dataStream, zipStr, ',');
    record.zip = atoi(zipStr.c_str());

    getline(dataStream, record.place_name, ',');
    getline(dataStream, record.state, ',');
    getline(dataStream, record.county, ',');

    getline(dataStream, latStr, ',');
    record.latitude = strtod(latStr.c_str(), nullptr);

    getline(dataStream, lonStr);
    record.longitude = strtod(lonStr.c_str(), nullptr);

    record.length = dataStr.length();

    return true;
}

struct StateExtremotes
{
    string state;
    buffer easternmost;
    buffer westernmost;
    buffer northernmost;
    buffer southernmost;
    bool initialized = false;
};

map<string, StateExtremotes> stateData;

void generateStateTable(vector<buffer>& records)
{
    stateData.clear();
    for (auto& record : records)
    {
        string state = record.state;
        if (state.empty()) continue;

        if (stateData.find(state) == stateData.end())
        {
            StateExtremotes extremes;
            extremes.state = state;
            extremes.easternmost = extremes.westernmost = extremes.northernmost = extremes.southernmost = record;
            extremes.initialized = true;
            stateData[state] = extremes;
        }
        else
        {
            StateExtremotes& extremes = stateData[state];
            if (record.longitude < extremes.easternmost.longitude) extremes.easternmost = record;
            if (record.longitude > extremes.westernmost.longitude) extremes.westernmost = record;
            if (record.latitude > extremes.northernmost.latitude) extremes.northernmost = record;
            if (record.latitude < extremes.southernmost.latitude) extremes.southernmost = record;
        }
    }
}

void printStateTable()
{
    printf("%-5s %-15s %-15s %-15s %-15s\n",
           "State", "Easternmost", "Westernmost", "Northernmost", "Southernmost");
    printf("%-5s %-15s %-15s %-15s %-15s\n",
           "-----", "---------------", "---------------", "---------------", "---------------");

    for (auto& pair : stateData)
    {
        StateExtremotes& extremes = pair.second;
        printf("%-5s %-15u %-15u %-15u %-15u\n",
               extremes.state.c_str(),
               extremes.easternmost.zip,
               extremes.westernmost.zip,
               extremes.northernmost.zip,
               extremes.southernmost.zip);
    }
}

bool readRecordAtOffset(const std::string& filename, std::streampos offset, buffer& outRecord)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return false;

    in.clear();
    in.seekg(offset);
    if (!in.good()) return false;

    std::string line;
    if (!std::getline(in, line)) return false;
    in.close();
    return unpackRecord(line, outRecord);
}

void writeHeaderRecord(std::ofstream& outStream, const std::string& headerText)
{
    if (!outStream.is_open()) return;
    outStream << headerText.length() << "," << headerText << "\n";
}

bool readHeaderLine(std::ifstream& inStream, std::string& outHeader)
{
    if (!inStream.is_open()) return false;
    inStream.clear();
    inStream.seekg(0);
    return static_cast<bool>(std::getline(inStream, outHeader));
}
