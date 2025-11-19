
#ifndef SIMPLEINDEX_H
#define SIMPLEINDEX_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

using namespace std; 

/**
 * @class simpleIndex
 *  @brief Implements a simple index structure mapping highest keys to block numbers.
 * This class provides functionalities to build the index from a data file,
 * write the index to a file, read the index from a file, dump the index contents
 * and find the block number for a given key.
 */
class simpleIndex 
{
    private:
        vector<uint32_t> highestKeys;//vector to store highest keys
        vector<int> RBN;//RBN = relative block number. vector to store highest keys and corresponding RBNs
    
    public:
        simpleIndex(); //constructor
        void buildIndex(const string& dataFilename, const string& indexFilename);//build the index from data file
        bool writeToFile(const string& indexFilename);//write index to file
        bool readFromFile(const string& indexFilename); //load index from file
        void dump() const; //dump the index contents
        int findBlock(uint32_t key) const;//find the block for the given key. 
};

#endif
