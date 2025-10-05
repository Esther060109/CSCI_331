#ifndef BUFFER_H 
#define BUFFER_H

#include <iostream> 
#include <string> 
#include <fstream> 
#include <sstream> 
#include <vector> 

using namespace std; 

/*
    The structure declares the variables of the zip, place name
    , state, county, lat, and long.
*/
typedef struct 
{ 
    unsigned int zip; 
    string place_name; 
    string state;
    string county; 
    double latitude; 
    double longitude; 
    string tempString=",";//initializing the tempString.  

}buffer;

struct display 
{ 
    public: 
        // No constructor needed - this is a utility class
        
        void print(buffer* pointer) 
        { 
            printf("zip code: %u, ", pointer->zip); 
            printf("Name of the place: %s, ", pointer->place_name.c_str()); 
            printf("State: %s, ", pointer->state.c_str()); 
            printf("County: %s, ", pointer->county.c_str()); 
            printf("Latitude: %.4f, ", pointer->latitude);
            printf("Longitude: %.4f\n", pointer->longitude);
        }
};

void parsing (int argc, char** argv, buffer* pointer, string file); //pointer is the pointer pointing to the buffer struct. 
void print(buffer* pointer); 
#endif