#include <managers/LanguageManager.h>
#include <persistence/JsonLoader.h>
#include <extensions/StringExtensions.h>

#include <map>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <locale>
#endif

std::map<std::string, Json::Value> LanguageManager::dictionary = {};

std::string LanguageManager::getSystemLanguage()
{
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
    LCID lcid = GetThreadLocale();
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
    if (LCIDToLocaleName(lcid, localeName, LOCALE_NAME_MAX_LENGTH, 0) == 0)
        return "en-us";

    return StringExtensions::toLowerCase(StringExtensions::convertWideToUtf8(localeName));
#else
    std::string localeName1 = std::setlocale(LC_ALL, "");
    std::string localeP = localeName1.substr(0, localeName1.find('.'));
    return StringExtensions::toLowerCase(localeP);
#endif
}

void LanguageManager::loadLang(const std::string locale, bool exitOnFailure)
{
    const std::string LANGUAGE_FOLDER = "/lang/";
    const std::string APPLICATION_FOLDER = JsonLoader::getApplicationFolder();

    if (dictionary.find(locale) != dictionary.end())
        return;

    std::cout << "Searching for " + locale + ".json" << std::endl;
    
    Json::Value lang;

    if (!JsonLoader::load(APPLICATION_FOLDER + LANGUAGE_FOLDER + locale + ".json", lang))
    {
        if (exitOnFailure)
            exit(1);

        loadLang();

        // Use en-us instead
        lang = dictionary["en-us"];
    }

    dictionary[locale] = lang;
}

std::string LanguageManager::getString(std::string key, const std::string locale)
{
    loadLang(locale, false);

    Json::Value value = JsonLoader::getNode(dictionary[locale], key);

    return (value == NULL || value.isNull()) ? "" : value.asString();
}

std::vector<std::string> LanguageManager::getArray(std::string key, const std::string locale)
{
    loadLang(locale, false);

    Json::Value value = JsonLoader::getNode(dictionary[locale], key);

    std::vector<std::string> array;

    if (value == NULL || value.isNull())
        return array;

    for (unsigned int i = 0; i < static_cast<unsigned int>(value.size()); i++)
        array.push_back(value[i].asString());
    
    return array;
}
