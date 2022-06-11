#pragma once

#include <string>
#include <vector>

class StringExtensions
{
public:
    static void findAndReplaceAll(std::string &data, const std::string toSearch, const std::string replaceStr);
    static void findAndReplaceAll(std::string &data, const std::vector<std::string> toSearch, const std::vector<std::string> replaceStr);
    static std::string toLowerCase(std::string value);
#if defined(_WIN32) || defined(_WIN64)
    static std::string convertWideToUtf8(const std::wstring &wstr);
    static std::wstring convertUtf8ToWide(const std::string &str);
#endif
};