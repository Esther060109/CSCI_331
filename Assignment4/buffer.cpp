/**
 * @file buffer.cpp
 * @brief Implements CSV parsing, sorting, length-indicated processing, 
 *        state extreme calculations, and header/record utilities.
 * 
 * Contains all major functionality for the ZIP Code Processing system.
 */
#include "buffer.h"
#include <cstdlib>
#include <algorithm>

using namespace std;

/**
 * @brief Parses a CSV file into buffer records, sorts them, and writes output.
 *
 * This function:
 *   - Reads a CSV file line-by-line
 *   - Parses each column into a buffer record
 *   - Stores all records into a vector
 *   - Sorts them first by ZIP, then by latitude
 *   - Writes sorted results to an output text file
 *
 * @param argc Command-line argument count (unused).
 * @param argv Command-line arguments (unused).
 * @param pointer Pointer to a buffer struct reused for each parsed row.
 * @param file Name of the CSV file to parse.
 * @param txtFile Output text file where sorted results are written.
 */
void parsing(int argc, char** argv, buffer* pointer, string file, ofstream& txtFile)
{
    // Write CSV header to output with length prefix
    string header = "zip,place_name,state,county,latitude,longitude";
    txtFile << header.length() << "," << header << endl;

    // Storage for all parsed records
    vector<buffer> records;

    ifstream inputFile(file);
    string line;

    // Skip first three header lines in the input CSV
    getline(inputFile, line); // skip headers
    getline(inputFile, line);
    getline(inputFile, line);

    // CSV Parsing Loop
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

    // Sorting
    sortingZip(records);
    sortingLocation(records);

    // Output sorted data
    for (const auto& record : records)
    {
        txtFile << record.length << "," << record.zip << "," << record.place_name << ","
                << record.state << "," << record.county << "," << record.latitude << "," << record.longitude << endl;
    }
}

/**
 * @brief Sorts ZIP code records numerically by ZIP code.
 *
 * @param records Vector of ZIP records to sort.
 */
void sortingZip(vector<buffer>& records)
{
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.zip < b.zip;
    });
}

/**
 * @brief Sorts ZIP code records by latitude (south → north).
 *
 * @param records Vector of ZIP code records.
 */
void sortingLocation(vector<buffer>& records)
{
    sort(records.begin(), records.end(), [](const buffer& a, const buffer& b) {
        return a.latitude < b.latitude;
    });
}

/**
 * @brief Reads all length-indicated records from a file into memory.
 *
 * Skips the first line (header) and unpacks each record.
 *
 * @param filename Input file name.
 * @param records Output vector filled with unpacked records.
 */
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

/**
 * @brief Unpacks a single length-indicated record.
 *
 * Length-indicated format:
 *   LENGTH,zip,place_name,state,county,latitude,longitude
 *
 * @param line Raw record string.
 * @param record Output buffer containing parsed values.
 * @return true if successful, false otherwise.
 */
bool unpackRecord(string line, buffer& record)
{
    if (line.empty()) return false;

    stringstream ss(line);
    string lengthStr, zipStr, latStr, lonStr;

    // length field
    getline(ss, lengthStr, ',');
    //int expectedLength = atoi(lengthStr.c_str());

    // Extract substring containing actual CSV data
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

/**
 * @struct StateExtremotes
 * @brief Holds the most extreme ZIP code positions for each state.
 *
 * Tracks:
 *  - Easternmost (lowest longitude)
 *  - Westernmost (highest longitude)
 *  - Northernmost (highest latitude)
 *  - Southernmost (lowest latitude)
 */
struct StateExtremotes
{
    string state;
    buffer easternmost;
    buffer westernmost;
    buffer northernmost;
    buffer southernmost;
    bool initialized = false;
};

// Global table storing per-state extremes.
map<string, StateExtremotes> stateData;

/**
 * @brief Computes regional extremes for each U.S. state.
 *
 * Examines each record, updating:
 *  - Easternmost
 *  - Westernmost
 *  - Northernmost
 *  - Southernmost
 *
 * @param records List of ZIP code records.
 */
void generateStateTable(vector<buffer>& records)
{
    stateData.clear();
    for (auto& record : records)
    {
        string state = record.state;
        if (state.empty()) continue;

        // Initialize state
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

/**
 * @brief Prints the previously computed per-state extreme table.
 */
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

/**
 * @brief Reads a specific length-indicated record at a given byte offset.
 *
 * Enables random-access retrieval.
 *
 * @param filename File to read from.
 * @param offset Byte position in the file.
 * @param outRecord Output record structure.
 * @return true if successful, false if failure or EOF.
 */
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

/**
 * @brief Writes a header text line with a length prefix.
 *
 * Format:
 *   length,header_text
 *
 * @param outStream Output stream.
 * @param headerText The header line to write.
 */
void writeHeaderRecord(std::ofstream& outStream, const std::string& headerText)
{
    if (!outStream.is_open()) return;
    outStream << headerText.length() << "," << headerText << "\n";
}

/**
 * @brief Reads the first (header) line from a length-indicated file.
 *
 * @param inStream Input file stream.
 * @param outHeader Header text (without length prefix).
 * @return true if header is read, false otherwise.
 */
bool readHeaderLine(std::ifstream& inStream, std::string& outHeader)
{
    if (!inStream.is_open()) return false;
    inStream.clear();
    inStream.seekg(0);
    return static_cast<bool>(std::getline(inStream, outHeader));
}