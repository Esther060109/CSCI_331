#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include "ZipRecord.h" 
using namespace std;

class Buffer
{
    public: 
        explicit Buffer(const string & path); 
        virtual ~Buffer();
        bool read(ZipRecord & record);

    private:
        //ifstream fin;
        bool headerSkipped; 
        //static bool parseLine(const string &line, vector<string> &out);
        static string trim(const string &s);
};
#endif // BUFFER_H