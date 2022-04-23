#include <iostream>
#include <codecvt>
#include <string>
#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace Extensions
{
    static void findAndReplaceAll(std::string &data, std::string toSearch, std::string replaceStr)
    {
        // Get the first occurrence
        size_t pos = data.find(toSearch);
        // Repeat till end is reached
        while (pos != std::string::npos)
        {
            // Replace this occurrence of Sub String
            data.replace(pos, toSearch.size(), replaceStr);
            // Get the next occurrence from the current position
            pos = data.find(toSearch, pos + replaceStr.size());
        }
    }

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable : 4267)
    static std::string ConvertWideToUtf8(const std::wstring &wstr)
    {
        int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
        std::string str(count, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
        return str;
    }

    static std::wstring ConvertUtf8ToWide(const std::string &str)
    {
        size_t count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
        return wstr;
    }

#pragma warning(pop)
#endif

    static std::string toLowerCase(std::string data)
    {
        std::transform(data.begin(), data.end(), data.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return data;
    }
}