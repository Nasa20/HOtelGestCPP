#ifndef SECURITY_H
#define SECURITY_H

#include <string>
#include <functional>
#include <sstream>
#include <iomanip>

using namespace std;

class Security {
public:
    // Generates a secure hash from the password using a Salt
    static string hashPassword(const string& password) {
        // 1. Add a "Salt" (random secret string) to prevent Rainbow Table attacks
        // CHANGE THIS STRING TO SOMETHING UNIQUE FOR YOUR PROJECT
        string salted = password + "H0t3l_M0r0cc0_S3cur1ty_S@lt_2026";
        
        // 2. Hash the salted string using standard C++ hasher
        hash<string> hasher;
        size_t hashValue = hasher(salted);
        
        // 3. Convert the numeric hash to a Hexadecimal String for storage
        stringstream ss;
        ss << hex << setfill('0') << setw(16) << hashValue;
        return ss.str();
    }
};

#endif