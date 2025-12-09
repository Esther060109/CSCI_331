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

int main(int argc, char** argv) {
    std::cout << "=== Processing Regular CSV File ===\n" << std::endl;

    buffer buf;  
    std::string regularCSV   = "us_postal_codes.csv";        
    std::string randomizedCSV = "us_postal_codes_randomized.csv";

    std::ofstream txtFileRandom("txtFileRandom.txt");
    if (!txtFileRandom.is_open()) {
        std::cerr << "Error: Cannot open output file txtFileRandom.txt\n";
        return 1;
    }

    // Parse CSV and write length-indicated text file
    parsing(argc, argv, &buf, randomizedCSV, txtFileRandom);
    txtFileRandom.close();

    std::cout << "\n=== Processing Randomized CSV File ===" << std::endl;

    std::vector<buffer> unpackedRecords;
    readLengthIndicatedFile("txtFileRandom.txt", unpackedRecords);

    std::cout << "\nFirst 3 unpacked records:" << std::endl;
    for (size_t i = 0; i < 3 && i < unpackedRecords.size(); ++i) {
        const buffer& rec = unpackedRecords[i];
        std::cout << "ZIP " << rec.zip << ": " 
             << rec.place_name << ", " << rec.state << ", " << rec.county
             << " (" << rec.latitude << ", " << rec.longitude << ")" << std::endl;
    }

    std::cout << "\n=== GENERATING STATE ANALYSIS TABLE ===" << std::endl;
    generateStateTable(unpackedRecords);
    printStateTable();

    std::cout << "\n=== GENERATING BLOCKED SEQUENCE SET FILE ===" << std::endl;
    std::string blockedFileName = "BlockedSequenceSet.dat";

    // Create BSS with only the filename (as constructor expects)
    BlockedSequenceSet bss(blockedFileName);

    // Add CSV-converted string records to BSS
    for (auto& rec : unpackedRecords) {
        std::string recStr = to_string(rec.zip) + "," + rec.place_name + "," + rec.state + "," +
                        rec.county + "," + to_string(rec.latitude) + "," + to_string(rec.longitude);
        bss.AddRecord(recStr);
    }

    bss.WriteToFile();
    bss.PrintSummary();
    
    // === Build B+Tree from the sequence set ===
    BPlusTree bptree(blockedFileName, 512);   // use same block size as blocks

    // Insert all records into the B+Tree's internal sequence set
    for (auto& rec : unpackedRecords) {
        std::string recStr = to_string(rec.zip) + "," + rec.place_name + "," + rec.state + "," +
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
    std::string stateKey = "FL"; 
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
