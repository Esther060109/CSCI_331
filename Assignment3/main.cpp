#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits> // for std::numeric_limits

#include "buffer.h"
#include "Block.h"
#include "BlockedSequenceSet.h"
#include "HeaderRecord.h"
#include "PrimaryKeyIndex.h"

using namespace std;

int main(int argc, char** argv) {
    cout << "=== Processing Regular CSV File ===\n" << endl;

    buffer buf;  
    string regularCSV = "us_postal_codes.csv";        
    string randomizedCSV = "us_postal_codes_randomized.csv";

    ofstream txtFileRandom("txtFileRandom.txt");
    if (!txtFileRandom.is_open()) {
        cerr << "Error: Cannot open output file txtFileRandom.txt\n";
        return 1;
    }

    parsing(argc, argv, &buf, randomizedCSV, txtFileRandom);
    txtFileRandom.close();

    cout << "\n=== Processing Randomized CSV File ===" << endl;

    vector<buffer> unpackedRecords;
    readLengthIndicatedFile("txtFileRandom.txt", unpackedRecords);

    cout << "\nFirst 3 unpacked records:" << endl;
    for (size_t i = 0; i < 3 && i < unpackedRecords.size(); ++i) {
        const buffer& rec = unpackedRecords[i];
        cout << "ZIP " << rec.zip << ": " 
             << rec.place_name << ", " << rec.state << ", " << rec.county
             << " (" << rec.latitude << ", " << rec.longitude << ")" << endl;
    }

    cout << "\n=== GENERATING STATE ANALYSIS TABLE ===" << endl;
    generateStateTable(unpackedRecords);
    printStateTable();

    cout << "\n=== GENERATING BLOCKED SEQUENCE SET FILE ===" << endl;
    string blockedFileName = "BlockedSequenceSet.dat";

    // Create BSS with only the filename (as constructor expects)
    BlockedSequenceSet bss(blockedFileName);

    // Add CSV-converted string records to BSS
    for (auto& rec : unpackedRecords) {
        string recStr = to_string(rec.zip) + "," + rec.place_name + "," + rec.state + "," +
                        rec.county + "," + to_string(rec.latitude) + "," + to_string(rec.longitude);
        bss.AddRecord(recStr);
    }

    bss.WriteToFile();
    bss.PrintSummary();
    
    return 0;
}
