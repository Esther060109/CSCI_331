/**
 * @file main.cpp
 * @brief Application program for "Zip Code 1.0" group project.
 *
 * Reads a CSV produced from the provided XLSX and prints, per state (alphabetical),
 * the easternmost (min longitude), westernmost (max longitude), northernmost (max latitude),
 * and southernmost (min latitude) ZIP codes.
 *
 * Build (example):
 *    g++ -std=c++17 -O2 -Wall -Wextra -o zip_app main.cpp bufferFile.cpp
 *
 * Run (example):
 *    ./zip_app us_postal_codes.csv
 *
 * Script capture (Linux/macOS):
 *    script -q run_zip_by_zip.txt ./zip_app us_postal_codes_by_zip.csv
 *    script -q run_zip_by_city.txt ./zip_app us_postal_codes_by_city.csv
 */

#include <iostream>
#include <iomanip>
#include <map>
#include <limits>
#include <string>
#include "bufferFile.h"

struct Extremes {
    // Track values and the corresponding ZIP
    double minLng =  std::numeric_limits<double>::infinity();
    std::string eastZip;
    double maxLng = -std::numeric_limits<double>::infinity();
    std::string westZip;

    double maxLat = -std::numeric_limits<double>::infinity();
    std::string northZip;
    double minLat =  std::numeric_limits<double>::infinity();
    std::string southZip;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <us_postal_codes.csv>\n";
        return 1;
    }

    const std::string csvPath = argv[1];
    BufferFile buf(csvPath);
    if (!buf.isOpen()) {
        std::cerr << "ERROR: Could not open or parse CSV header: " << csvPath << "\n";
        return 2;
    }

    std::map<std::string, Extremes> byState;
    ZipRecord rec;
    size_t count = 0;
    while (buf.getNextRecord(rec)) {
        ++count;
        auto& ex = byState[rec.state];
        // East = least longitude
        if (rec.longitude < ex.minLng) { ex.minLng = rec.longitude; ex.eastZip = rec.zip; }
        // West = greatest longitude
        if (rec.longitude > ex.maxLng) { ex.maxLng = rec.longitude; ex.westZip = rec.zip; }
        // North = greatest latitude
        if (rec.latitude > ex.maxLat) { ex.maxLat = rec.latitude; ex.northZip = rec.zip; }
        // South = least latitude
        if (rec.latitude < ex.minLat) { ex.minLat = rec.latitude; ex.southZip = rec.zip; }
    }

    if (byState.empty()) {
        std::cerr << "No data rows were read. Check that the CSV has a header and rows.\n";
        return 3;
    }

    // Header row
    std::cout << "State,EastZip,WestZip,NorthZip,SouthZip\n";
    for (const auto& [state, ex] : byState) {
        std::cout << state << ","
                  << ex.eastZip << ","
                  << ex.westZip << ","
                  << ex.northZip << ","
                  << ex.southZip << "\n";
    }

    return 0;
}
