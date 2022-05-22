/*
 * Dimitrios Koropoulis 3967
 * csd3967@csd.uoc.gr
 * CS352 - Fall 2021
 * util.cc
 */

#include <sstream>
#include <algorithm>
#include "util.h"

bool is_float(std::string str) {

    std::istringstream iss(str);
    float f;

    iss >> std::noskipws >> f;

    /* Check if the entire string was consumed and if either failbit or badbit is set */
    return iss.eof() && !iss.fail();
}

bool is_int(std::string str) {
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool is_bool(std::string str) {
    return str == "true" || str == "false";
}
