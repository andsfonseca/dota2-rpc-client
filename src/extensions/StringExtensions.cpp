#include <extensions/StringExtensions.h>

#include <algorithm>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

void StringExtensions::findAndReplaceAll(std::string &data, const std::string toSearch, const std::string replaceStr)
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

void StringExtensions::findAndReplaceAll(std::string &data, const std::vector<std::string> toSearch, const std::vector<std::string> replaceStr)
{
    size_t size = toSearch.size();
    if (size != replaceStr.size())
        return;

    for (int i = 0; i < size; i++)
        findAndReplaceAll(data, toSearch[i], replaceStr[i]);
}

std::string StringExtensions::toLowerCase(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
    return value;
}

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning(disable : 4267)
std::string StringExtensions::convertWideToUtf8(const std::wstring &wstr)
{
    int count = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL);
    std::string str(count, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], count, NULL, NULL);
    return str;
}

std::wstring StringExtensions::convertUtf8ToWide(const std::string &str)
{
    size_t count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), NULL, 0);
    std::wstring wstr(count, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), &wstr[0], count);
    return wstr;
}

#pragma warning(pop)
#endif
