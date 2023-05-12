#include <cconversion.h>
#include <string>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>

unsigned char CConversion::hexchar_to_int(char const ch)
{
   if (ch >= '0' && ch <= '9') return ch - '0';
   if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
   if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
      throw std::invalid_argument("Invalid hexadecimal character");
}
std::vector<unsigned char> CConversion::hexstr_to_bytes(std::string str)
{
   std::vector<unsigned char> result;
   for (size_t i = 0; i < str.size(); i += 2)
   {
      result.push_back(
         (hexchar_to_int(str[i]) << 4) | hexchar_to_int(str[i+1]));
   }
   return result;
}
std::vector<std::string> CConversion::split(const std::string& s, char delim, std::vector<std::string>& elems, bool bNoEmpty)
{
    std::stringstream ss(s);
    std::string item;


    for(std::string item; std::getline(ss, item, delim);)
      if (  (bNoEmpty == false)
          ||(item.length() > 0))
        elems.push_back(item);
    return elems;
}
std::vector<std::string> CConversion::Split(const char* pattern, char delim)
{
    std::vector<std::string> elems;
    split(pattern, delim, elems);
    return elems;
}
std::string CConversion::ltrim(std::string s)
{
    std::string sWhitespace = " \t\f\v";
    size_t start = s.find_first_not_of(sWhitespace);
    return (start == std::string::npos)? "" : s.substr(start);
}
std::string CConversion::rtrim(std::string s)
{
    std::string sWhitespace = " \t\f\v";
    size_t end = s.find_last_not_of(sWhitespace);
    return (end == std::string::npos)?"": s.substr(0, end + 1);
}
std::string CConversion::trim(std::string s)
{
    return ltrim(rtrim(s));
}

