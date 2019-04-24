#include <codecvt>
#include <locale>
#include "StringHelpers.h"
using namespace std;

// a case-insensitive comparison function:
bool mycomp (char c1, char c2)
{ return std::tolower(c1)<std::tolower(c2); }

bool CompareStringInsensitive(const std::string &str1, const std::string &str2)
{
    return std::lexicographical_compare(str1.begin(), str1.end(), str2.begin(), str2.end(), mycomp) == false &&
    std::lexicographical_compare(str2.begin(), str2.end(), str1.begin(), str1.end(), mycomp) == false;
}

int CompareString(const std::string &str1, const std::string &str2)
{
	return str1.compare(str2);
}

std::string ConvertFromUtf16ToUtf8(const std::wstring& wstr) 
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

std::wstring ConvertFromUtf8ToUtf16(const std::string& str) 
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

void Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
    // lastPos is the first character in the next token to be returned
    string::size_type lastPos = 0;
    
    // pos is the location of the next delimiter
    string::size_type pos = str.find_first_of(delimiters, lastPos);
    
    while (lastPos != string::npos)
    {
        // Found a token, add it to the vector.
        if(pos == string::npos)
        {
            // No more tokens, return the rest of the string
            tokens.push_back(str.substr(lastPos));
            lastPos = string::npos;
        }
        else
        {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = pos + 1;
            pos = str.find_first_of(delimiters, lastPos);
        }
    }
    
    // An empty string passed in is one empty token
    if(tokens.size() == 0)
    {
        tokens.push_back("");
    }
}
