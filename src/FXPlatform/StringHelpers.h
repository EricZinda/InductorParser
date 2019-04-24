#pragma once
#include <vector>

std::string ConvertFromUtf16ToUtf8(const std::wstring& wstr); 
std::wstring ConvertFromUtf8ToUtf16(const std::string& str);

// Returns true if the strings are tolower() equivalent, false otherwise
bool CompareStringInsensitive(const std::string &str1, const std::string &str2);
int CompareString(const std::string &str1, const std::string &str2);
void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
