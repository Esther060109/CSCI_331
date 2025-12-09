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
#include "BPlusTree.h"

using namespace std;

int main(int argc, char** argv) {
    cout << "=== Processing Regular CSV File ===\n" << endl;

    buffer buf;  
    string regularCSV   = "us_postal_codes.csv";        
    string randomizedCSV = "us_postal_codes_randomized.csv";

    ofstream txtFileRandom("txtFileRandom.txt");
    if (!txtFileRandom.is_open()) {
        cerr << "Error: Cannot open output file txtFileRandom.txt\n";
        return 1;
    }

    // Parse CSV and write length-indicated text file
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
    
    // === Build B+Tree from the sequence set ===
    BPlusTree bptree(blockedFileName, 512);   // use same block size as blocks

    // Insert all records into the B+Tree's internal sequence set
    for (auto& rec : unpackedRecords) {
        string recStr = to_string(rec.zip) + "," + rec.place_name + "," + rec.state + "," +
                        rec.county + "," + to_string(rec.latitude) + "," + to_string(rec.longitude);
        bptree.Insert(recStr);
    }

    // Build static B+tree index and dump its structure
    bptree.BuildStaticIndex();
    bptree.DumpTree(std::cout);

    // Print a simple summary of the tree
    bptree.PrintSummary();  

    // --- Test searching a ZIP code via the B+tree index ---
    std::string result;
    std::string key = "90210";   // example ZIP
    if (bptree.Search(key, result)) {
        std::cout << "\nFound ZIP " << key << ": " << result << "\n";
    } else {
        std::cout << "\nZIP " << key << " not found\n";
    }
    
    // --- Test searching by State using the B+tree file as a sequence set ---
    std::string stateKey = "MN"; 
    std::vector<std::string> stateResults;
    bptree.SearchByState(stateKey, stateResults);

    std::cout << "\nRecords for state " << stateKey << ":\n";

    int printed = 0; 
    for (const std::string& recStr : stateResults) {
        std::cout << recStr << "\n";
        printed++;
        if (printed >= 5) {  // limit printed records for readability
            std::cout << "...and " << (stateResults.size() - printed)
                      << " more records.\n";
            break;
        }
    }

    if (printed == 0) {
        std::cout << "No records found for state " << stateKey << ".\n";
    }

    return 0;
}
