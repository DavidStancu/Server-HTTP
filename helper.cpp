#include "helper.h"
#include <iostream>
#include <cctype>
using namespace std;

// prompts the user for each field name and collects the input
vector<string> promptCredentials(const vector<string>& fields) {   
    vector<string> vals;
    for (auto& f : fields) {
        cout << f << "=" << flush;
        string line;
        if (!getline(cin, line)) 
            break;
        vals.push_back(line);
    }
    return vals;
}

//returns true is s contains only digits
bool validateNumber(const string& s) {
    if (s.empty())
        return false;
    for (char c : s) 
        if (!isdigit(c)) 
            return false;
    return true;
}

//returns true if s if of a valid form of double
bool validateDouble(const string& s) {
    if (s.empty()) return false;
    bool dot = false;
    for (char c : s) {
        if (c == '.') {
            if (dot) 
                return false;
            dot = true;
        } else if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}
