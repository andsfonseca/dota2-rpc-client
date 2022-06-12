#pragma once

#include <json/json.h>
#include <string>
#include <map>
#include <vector>

class LanguageManager
{
public:
    static std::string getSystemLanguage();
    static std::string getString(std::string key, const std::string locale = "en-us");
    static std::vector<std::string> getArray(std::string key, const std::string locale = "en-us");

private:
    static void loadLang(const std::string locale = "en-us", const bool exitOnFailure = true);

protected:
    static std::map<std::string, Json::Value> dictionary;
};