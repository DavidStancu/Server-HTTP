#pragma once
#include <vector>
#include <string>

std::vector<std::string> promptCredentials(const std::vector<std::string>& fields);
bool validateNumber(const std::string& s);
bool   validateDouble(const std::string& s);