
#include "SimpleIndex.h"
#include <iostream> 
#include <string>
#include <vector> 
#include <fstream>
#include <utility>
#include <cstdint>

#include "BlockedSequenceSet.h"
#include "Block.h"
#include "buffer.h"


using namespace std; 

simpleIndex::simpleIndex() 
{
    BlockedSequenceSet bss("BlockedSequenceSetData.txt");
    
    for(const Block& block: bss.getBlocks()) 
    {
        string lastRecord = block.getRecords().back();
        uint32_t highestKey = stoi(lastRecord.substr(0, lastRecord.find(',')));
        highestKeys.push_back(highestKey);
        RBN.push_back(block.GetRBN());
    }

    highestKeys.push_back(highestKeys.back() + 1); // Sentinel value


}

void simpleIndex::buildIndex(const string& dataFilename, const string& indexFilename)
{
    BlockedSequenceSet bss(dataFilename);
    
    for(const Block& block: bss.getBlocks()) 
    {
        uint32_t highestKey = 0; 

        for (const string& record: block.getRecords())
        { 
            buffer rec; 
            if(unpackRecord(record, rec))
            { 
                if (rec.zip > highestKey) 
                {
                    highestKey = rec.zip; 
                }       
            }
        }
        highestKeys.push_back(highestKey);
        RBN.push_back(block.GetRBN());
    }
    writeToFile(indexFilename);
}

bool simpleIndex::writeToFile(const string& indexFilename)
{
    ofstream out(indexFilename); 
    if(!out.is_open()) 
    {
        cerr<< "Error: cannot create index file " << indexFilename << '\n';
        return false;
    }
    for(size_t i = 0; i< highestKeys.size(); ++i) 
    {
        out << highestKeys[i] << "," << RBN[i] << '\n';
    }
    out.close();
    return true;
};

bool simpleIndex::readFromFile(const string& indexFilename)
{
    ifstream in(indexFilename); 
    if(!in.is_open()) 
    {
        cerr << "Error: cannot open index file " << indexFilename << '\n';
        return false;
    }
    highestKeys.clear();
    RBN.clear();

    string line; 
    while(getline(in, line)) 
    {
        if(line.empty()) continue; 
        size_t commaPos = line.find(',');
        if(commaPos == string::npos) continue;

        uint32_t key = static_cast<uint32_t>(stoul(line.substr(0, commaPos)));
        int rbn = stoi(line.substr(commaPos + 1));

        highestKeys.push_back(key);
        RBN.push_back(rbn);
    }
    in.close();
    return true;
};

void simpleIndex::dump() const
{
    std::cout << "Simple Index Dump (HighestKey, RBN):" << std::endl;
    for (size_t i = 0; i < highestKeys.size(); ++i) {
        std::cout << highestKeys[i] << ", " << RBN[i] << std::endl;
    }
};

int simpleIndex::findBlock(uint32_t key) const
{
    int left = 0;
    int right = highestKeys.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (key <= highestKeys[mid]) {
            if (mid == 0 || key > highestKeys[mid - 1]) {
                return RBN[mid];
            }
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1; // Key not found
};
