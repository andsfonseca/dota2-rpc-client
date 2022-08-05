#pragma once

#include <string>
#include <vector>

namespace ANSIColor {
    enum Code {
        FG_BLACK    = 30,
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_YELLOW   = 33,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_WHITE    = 47,
        BG_DEFAULT  = 49
    };
}

class StringExtensions
{
public:
    static void findAndReplaceAll(std::string &data, const std::string toSearch, const std::string replaceStr);
    static void findAndReplaceAll(std::string &data, const std::vector<std::string> toSearch, const std::vector<std::string> replaceStr);
    static std::string toLowerCase(std::string value);
    static std::string getValueAsANSIColourCodes(std::string value, const std::vector<ANSIColor::Code> codes);
#if defined(_WIN32) || defined(_WIN64)
    static std::string convertWideToUtf8(const std::wstring &wstr);
    static std::wstring convertUtf8ToWide(const std::string &str);
#endif
};