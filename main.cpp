#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <string>
#include <cstdlib>
#include "Location/Location.h"
#include "DelimFieldBuffer/Delim.h"

using namespace std;

/**
 * @struct StateExtremes
 * @brief Holds the easternmost, westernmost, northernmost, and southernmost
 *        Location records for a particular state.
 */
struct StateExtremes {
    Location eastMost;   ///< Location with the smallest longitude
    Location westMost;   ///< Location with the largest longitude
    Location northMost;  ///< Location with the largest latitude
    Location southMost;  ///< Location with the smallest latitude
    bool initialized = false; ///< Indicates whether any record has been stored
};

/**
 * @brief Reads the US postal code CSV file and prints a table of extremes per state.
 *
 * Reads the CSV file line by line, populates Location objects, determines the 
 * easternmost, westernmost, northernmost, and southernmost Zip Codes per state,
 * and prints the results in a formatted table with alphabetical state IDs.
 *
 * @return int Returns 0 on success, 1 if the CSV file cannot be opened.
 */
int main() {
    const char* filename = "us_postal_codes_CSV.csv";
    ifstream in(filename);
    if (!in) {
        cerr << "Cannot open CSV file: " << filename << endl;
        return 1;
    }

    DelimFieldBuffer buffer(','); // CSV is comma-delimited
    Location loc;

    // Skip CSV header line
    string headerLine;
    getline(in, headerLine);

    map<string, StateExtremes> stateMap;
    string line;

    while (getline(in, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string zip, place, state, county, lat, lon;

        // Parse CSV fields
        getline(ss, zip, ',');
        getline(ss, place, ',');
        getline(ss, state, ',');
        getline(ss, county, ',');
        getline(ss, lat, ',');
        getline(ss, lon, ',');

        // Populate Location object
        strcpy(loc.ZipCode, zip.c_str());
        strcpy(loc.PlaceName, place.c_str());
        strcpy(loc.State, state.c_str());
        strcpy(loc.County, county.c_str());
        strcpy(loc.Latitude, lat.c_str());
        strcpy(loc.Longitude, lon.c_str());

        if (strlen(loc.State) != 2) continue; // Skip invalid rows

        double dlat = atof(loc.Latitude);
        double dlon = atof(loc.Longitude);

        StateExtremes& s = stateMap[state];

        if (!s.initialized) {
            s.eastMost = s.westMost = s.northMost = s.southMost = loc;
            s.initialized = true;
        } else {
            if (dlon < atof(s.eastMost.Longitude)) s.eastMost = loc;
            if (dlon > atof(s.westMost.Longitude)) s.westMost = loc;
            if (dlat > atof(s.northMost.Latitude)) s.northMost = loc;
            if (dlat < atof(s.southMost.Latitude)) s.southMost = loc;
        }
    }
    in.close();

    // Print the table header
    cout << left << setw(6) << "State"
         << setw(14) << "Easternmost"
         << setw(14) << "Westernmost"
         << setw(14) << "Northernmost"
         << setw(14) << "Southernmost" << endl;
    cout << string(62, '-') << endl;

    // Print extremes per state in alphabetical order
    for (auto& pair : stateMap) {
        StateExtremes& s = pair.second;
        cout << left << setw(6) << pair.first
             << setw(14) << s.eastMost.ZipCode
             << setw(14) << s.westMost.ZipCode
             << setw(14) << s.northMost.ZipCode
             << setw(14) << s.southMost.ZipCode
             << endl;
    }

    return 0;
}