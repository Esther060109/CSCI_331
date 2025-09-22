/**
 * @file main.cpp
 * @brief Main program for Zip Code Group Project 1.0
 *
 * This program reads a CSV file containing zip-code level records (one record per row),
 * groups them by state (two-character state ID), and for each state reports:
 *   - Easternmost Zip Code (least longitude)
 *   - Westernmost Zip Code (greatest longitude)
 *   - Northernmost Zip Code (greatest latitude)
 *   - Southernmost Zip Code (least latitude)
 *
 * The program expects each record to contain at least:
 *   - a two-character state id (std::string) -> field name: "state" or "State"
 *   - a zip code (string)                     -> field name: "zip" or "Zip"
 *   - a latitude (double)                     -> field name: "latitude" or "lat"
 *   - a longitude (double)                    -> field name: "longitude" or "lon" or "lng"
 *
 * This file assumes the existence of a Buffer class and a ZipRecord type (see assumptions below).
 *
 * Usage:
 *   ./zip_by_state input_sorted_by_zip.csv
 *   ./zip_by_state file1.csv file2.csv   # compares output for two CSVs (sorted differently)
 *
 * If you don't yet have a Buffer class to test, see the "FALLBACK CSV PARSER" section below
 * (search for FALLBACK_CSV_PARSER) and enable it by defining USE_FALLBACK_CSV_PARSER.
 *
 * @author
 * @date
 */

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

/// Uncomment to use the fallback CSV parser built into this file (useful for testing).
// #define USE_FALLBACK_CSV_PARSER

#ifndef USE_FALLBACK_CSV_PARSER
// === Assumptions about external Buffer + ZipRecord API ===
//
// You must provide the following headers / types in your project:
//   - Buffer.h: declares a class Buffer which can be constructed from a filename and provides:
//         bool readNext(ZipRecord &out);   // reads next record; returns false at EOF
//     OR
//         std::vector<ZipRecord> Buffer::readAll(); // alternative API
//
//   - ZipRecord.h: declares a struct/class ZipRecord with at least the following public members:
//         std::string state;    // 2-character state ID (e.g., "IL")
//         std::string zip;      // zip code as string (leading zeros preserved)
//         double latitude;      // decimal degrees (positive north)
//         double longitude;     // decimal degrees (positive east) -- ensure negative for west longitudes
//
// The implementation below uses the readNext(Buffer) pattern. If your Buffer uses readAll(), adapt
// the code in loadRecordsFromBuffer() accordingly.
//
// Include your project's Buffer and ZipRecord definitions here:
#include "Buffer.h"
#include "ZipRecord.h"

#else
// === FALLBACK CSV PARSER ===
// If you don't have the Buffer class ready, the fallback will parse a CSV with headers
// and produce the same ZipRecord-like struct. This is intentionally conservative and robust.
//
// To use, define USE_FALLBACK_CSV_PARSER at top of this file (or pass -DUSE_FALLBACK_CSV_PARSER).
struct ZipRecord {
    std::string state;
    std::string zip;
    double latitude;
    double longitude;
    // You can add other fields here if you like; they won't be used by the program.
};

/**
 * @brief Simple CSV line splitter that handles quoted fields (rudimentary).
 *
 * This is not a full-fledged CSV parser but supports common cases with fields quoted by double quotes,
 * and commas inside quoted fields.
 */
static std::vector<std::string> splitCsvLine(const std::string &line) {
    std::vector<std::string> out;
    std::string cur;
    bool inQuote = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"' ) {
            if (inQuote && i + 1 < line.size() && line[i + 1] == '"') {
                // escaped quote
                cur.push_back('"');
                ++i;
            } else {
                inQuote = !inQuote;
            }
        } else if (c == ',' && !inQuote) {
            out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

/**
 * @brief Load CSV file into vector<ZipRecord>.
 *
 * The parser tries to locate the latitude/longitude/state/zip columns using common header names.
 * It is case-insensitive for header matching.
 *
 * @param path Path to CSV file
 * @param out Vector to append read ZipRecord rows
 * @return true on success, false on file open / header failure
 */
static bool loadRecordsFromCsvFile(const std::string &path, std::vector<ZipRecord> &out) {
    std::ifstream in(path);
    if (!in) {
        std::cerr << "Error: cannot open file: " << path << std::endl;
        return false;
    }
    std::string headerLine;
    if (!std::getline(in, headerLine)) {
        std::cerr << "Error: empty file: " << path << std::endl;
        return false;
    }
    auto headers = splitCsvLine(headerLine);
    // normalize headers to lowercase
    std::vector<std::string> headersLower;
    for (auto &h : headers) {
        std::string t = h;
        for (auto &ch : t) ch = static_cast<char>(std::tolower((unsigned char)ch));
        headersLower.push_back(t);
    }
    auto findHeader = [&](std::initializer_list<std::string> candidates)->int {
        for (auto &cand : candidates) {
            for (size_t i = 0; i < headersLower.size(); ++i) {
                if (headersLower[i] == cand) return static_cast<int>(i);
            }
        }
        return -1;
    };
    int idxState = findHeader({"state", "st", "state_id"});
    int idxZip = findHeader({"zip", "zipcode", "zip_code"});
    int idxLat = findHeader({"latitude", "lat"});
    int idxLon = findHeader({"longitude", "lon", "lng", "long"});
    if (idxState < 0 || idxZip < 0 || idxLat < 0 || idxLon < 0) {
        std::cerr << "Error: required headers not found in CSV: need state, zip, latitude, longitude." << std::endl;
        return false;
    }
    std::string line;
    size_t lineNo = 1;
    while (std::getline(in, line)) {
        ++lineNo;
        if (line.empty()) continue;
        auto cols = splitCsvLine(line);
        if (cols.size() <= static_cast<size_t>(std::max({idxState, idxZip, idxLat, idxLon}))) {
            // skip malformed row
            continue;
        }
        ZipRecord r;
        r.state = cols[idxState];
        r.zip = cols[idxZip];
        try {
            r.latitude = std::stod(cols[idxLat]);
            r.longitude = std::stod(cols[idxLon]);
        } catch (...) {
            // skip row if lat/lon not numeric
            continue;
        }
        // trim spaces from state
        r.state.erase(r.state.begin(), std::find_if(r.state.begin(), r.state.end(), [](unsigned char ch){ return !std::isspace(ch); }));
        r.state.erase(std::find_if(r.state.rbegin(), r.state.rend(), [](unsigned char ch){ return !std::isspace(ch); }).base(), r.state.end());
        if (r.state.size() != 2) {
            // still accept entries where state is not exactly 2-char, but normalize if possible
            // convert to uppercase
            std::string s = r.state;
            for (auto &c : s) c = static_cast<char>(std::toupper((unsigned char)c));
            r.state = s;
        } else {
            for (auto &c : r.state) c = static_cast<char>(std::toupper((unsigned char)c));
        }
        out.push_back(r);
    }
    return true;
}

#endif // USE_FALLBACK_CSV_PARSER

/**
 * @brief A small alias for the record type used by the rest of this file.
 *
 * If you use the external Buffer/ZipRecord, ZipRecAlias is the external ZipRecord type.
 * If using the fallback, ZipRecord above is used.
 */
#ifdef USE_FALLBACK_CSV_PARSER
using ZipRecAlias = ZipRecord;
#else
using ZipRecAlias = ZipRecord;
#endif

/**
 * @brief StateExtremes holds the four extreme records for a single state.
 *
 * We store pointers (or indexes) into the vector of ZipRecAlias to avoid copying large records.
 */
struct StateExtremes {
    const ZipRecAlias *eastmost = nullptr;   ///< least longitude (more east in western hemisphere with negative longitudes? see note)
    const ZipRecAlias *westmost = nullptr;   ///< greatest longitude
    const ZipRecAlias *northmost = nullptr;  ///< greatest latitude
    const ZipRecAlias *southmost = nullptr;  ///< least latitude
};

/**
 * @brief Update extremes for a state with a new record.
 *
 * @param extremes reference to StateExtremes to update
 * @param r pointer to record to consider
 */
static void updateExtremes(StateExtremes &extremes, const ZipRecAlias *r) {
    if (!r) return;
    // Eastmost = least longitude
    if (!extremes.eastmost || r->longitude < extremes.eastmost->longitude) {
        extremes.eastmost = r;
    }
    // Westmost = greatest longitude
    if (!extremes.westmost || r->longitude > extremes.westmost->longitude) {
        extremes.westmost = r;
    }
    // Northmost = greatest latitude
    if (!extremes.northmost || r->latitude > extremes.northmost->latitude) {
        extremes.northmost = r;
    }
    // Southmost = least latitude
    if (!extremes.southmost || r->latitude < extremes.southmost->latitude) {
        extremes.southmost = r;
    }
}

/**
 * @brief Load records from a CSV file using the project's Buffer API or fallback parser.
 *
 * This function is a thin wrapper that adapts either Buffer::readNext(...) or the fallback CSV
 * loader into a vector<ZipRecAlias>.
 *
 * @param path input CSV path
 * @param out vector to append records to
 * @return true on success, false otherwise
 */
static bool loadRecordsFromFile(const std::string &path, std::vector<ZipRecAlias> &out) {
#ifdef USE_FALLBACK_CSV_PARSER
    return loadRecordsFromCsvFile(path, out);
#else
    // Expected Buffer API:
    // Buffer buf(path);
    // ZipRecAlias rec;
    // while (buf.readNext(rec)) { out.push_back(rec); }
    //
    // If your Buffer has a different API (e.g., readAll()), modify this adapter.
    try {
        Buffer buf(path);
        ZipRecAlias rec;
        while (buf.readNext(rec)) {
            out.push_back(rec);
        }
        return true;
    } catch (const std::exception &ex) {
        std::cerr << "Error while reading file via Buffer: " << ex.what() << std::endl;
        return false;
    }
#endif
}

/**
 * @brief Compute state extremes from a vector of ZipRecAlias.
 *
 * @param rows input records
 * @return map from state id -> StateExtremes
 */
static std::map<std::string, StateExtremes> computeStateExtremes(const std::vector<ZipRecAlias> &rows) {
    std::map<std::string, StateExtremes> out;
    for (const auto &r : rows) {
        std::string state = r.state;
        // normalize state to uppercase and trim
        for (auto &c : state) c = static_cast<char>(std::toupper((unsigned char)c));
        if (state.empty()) continue;
        // if state more than 2 chars, still accept (some datasets use names); you may want to extract first 2 chars
        auto &ext = out[state];
        updateExtremes(ext, &r);
    }
    return out;
}

/**
 * @brief Print results table to stdout.
 *
 * Columns:
 *  State | Eastmost ZIP (lon) | Westmost ZIP (lon) | Northmost ZIP (lat) | Southmost ZIP (lat)
 *
 * @param extremes map from state->StateExtremes
 */
static void printResultsTable(const std::map<std::string, StateExtremes> &extremes) {
    // Header
    std::cout << std::left << std::setw(8) << "State"
              << std::setw(20) << "Eastmost (ZIP / Lon)"
              << std::setw(22) << "Westmost (ZIP / Lon)"
              << std::setw(22) << "Northmost (ZIP / Lat)"
              << std::setw(22) << "Southmost (ZIP / Lat)"
              << "\n";

    std::cout << std::string(8 + 20 + 22 + 22 + 22, '-') << "\n";

    // States are already sorted in std::map by lexical order (alphabetical).
    for (const auto &p : extremes) {
        const std::string &state = p.first;
        const auto &ext = p.second;

        auto printRec = [](const ZipRecAlias *r) -> std::string {
            if (!r) return std::string("-");
            std::ostringstream oss;
            oss << r->zip << " / " << std::fixed << std::setprecision(6) << (r->longitude) ;
            return oss.str();
        };
        auto printRecLat = [](const ZipRecAlias *r) -> std::string {
            if (!r) return std::string("-");
            std::ostringstream oss;
            oss << r->zip << " / " << std::fixed << std::setprecision(6) << (r->latitude) ;
            return oss.str();
        };

        std::cout << std::left << std::setw(8) << state
                  << std::setw(20) << printRec(ext.eastmost)
                  << std::setw(22) << printRec(ext.westmost)
                  << std::setw(22) << printRecLat(ext.northmost)
                  << std::setw(22) << printRecLat(ext.southmost)
                  << "\n";
    }
}

/**
 * @brief Compare two computed extremes maps for equality.
 *
 * Uses zip+lat+lon equality for each extreme. Returns true if equal for all states.
 *
 * @param a first map
 * @param b second map
 * @return true if identical, false otherwise
 */
static bool extremesEqual(const std::map<std::string, StateExtremes> &a, const std::map<std::string, StateExtremes> &b) {
    if (a.size() != b.size()) return false;
    for (const auto &p : a) {
        const std::string &state = p.first;
        auto it = b.find(state);
        if (it == b.end()) return false;
        auto getKey = [](const ZipRecAlias *r) -> std::tuple<std::string, double, double> {
            if (!r) return {"", NAN, NAN};
            return {r->zip, r->latitude, r->longitude};
        };
        auto ta = p.second;
        auto tb = it->second;
        if (getKey(ta.eastmost) != getKey(tb.eastmost)) return false;
        if (getKey(ta.westmost) != getKey(tb.westmost)) return false;
        if (getKey(ta.northmost) != getKey(tb.northmost)) return false;
        if (getKey(ta.southmost) != getKey(tb.southmost)) return false;
    }
    return true;
}

/**
 * @brief Program entry point.
 *
 * Accepts one or two command-line arguments:
 *   ./program file1.csv [file2.csv]
 * If file2 is provided the program compares the outputs of the two files and reports whether they are identical.
 *
 * @param argc number of arguments
 * @param argv arguments
 * @return exit code
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " input.csv [input_sorted_by_other_field.csv]\n";
        return 1;
    }
    std::string fileA = argv[1];
    std::string fileB;
    bool compareMode = false;
    if (argc >= 3) {
        fileB = argv[2];
        compareMode = true;
    }

    // Load file A
    std::vector<ZipRecAlias> rowsA;
    if (!loadRecordsFromFile(fileA, rowsA)) {
        std::cerr << "Failed to read file: " << fileA << std::endl;
        return 2;
    }
    auto extremesA = computeStateExtremes(rowsA);

    std::cout << "Results for file: " << fileA << "\n\n";
    printResultsTable(extremesA);

    if (compareMode) {
        std::vector<ZipRecAlias> rowsB;
        if (!loadRecordsFromFile(fileB, rowsB)) {
            std::cerr << "Failed to read file: " << fileB << std::endl;
            return 3;
        }
        auto extremesB = computeStateExtremes(rowsB);
        std::cout << "\nResults for file: " << fileB << "\n\n";
        printResultsTable(extremesB);

        bool identical = extremesEqual(extremesA, extremesB);
        std::cout << "\nComparison result: ";
        if (identical) {
            std::cout << "IDENTICAL: The two CSVs produced the same state extremes.\n";
        } else {
            std::cout << "DIFFER: The two CSVs produced different state extremes.\n";
        }
    }

    return 0;
}
