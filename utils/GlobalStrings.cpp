#include "StringExtensions.cpp"
#include <filesystem>
#include <fstream>
#include <vector>
#include <json/json.h>

#if defined(_WIN32) || defined(_WIN64)
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace GlobalStrings
{
    static Json::Value LocaleStrings;

    static std::string GetLocale()
    {
#if defined(_WIN32) || defined(_WIN64)
        LCID lcid = GetThreadLocale();
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
        if (LCIDToLocaleName(lcid, localeName, LOCALE_NAME_MAX_LENGTH, 0) == 0)
            return "en-us";

        return Extensions::toLowerCase(Extensions::ConvertWideToUtf8(localeName));
#else
        std::string localeName1 = std::setlocale(LC_ALL, "");
        std::string localeP = localeName1.substr(0, localeName1.find('.'));
        return Extensions::toLowerCase(localeP);
#endif
    }

    static std::string GetExecutableFolder()
    {
        std::string path;
#if defined(_WIN32) || defined(_WIN64)
        char result[MAX_PATH];
        path = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
        Extensions::findAndReplaceAll(path, "\\", "/");
#else
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        path = std::string(result, (count > 0) ? count : 0);
#endif
        size_t last_slash_idx = path.rfind('/');
        if (std::string::npos != last_slash_idx)
        {
            path = path.substr(0, last_slash_idx);
        }
        return path;
    }

    static void LoadLanguageJson()
    {
        const std::string LANGUAGE_FOLDER = "/lang/";
        const std::string LANGUAGE_DEFAULT = "en-us";

        std::string folder = GetExecutableFolder();
        std::string localeName = GetLocale();

        std::ifstream file(folder + LANGUAGE_FOLDER + localeName + ".json");
        if (!file)
        {
            file = std::ifstream(folder + LANGUAGE_FOLDER + LANGUAGE_DEFAULT + ".json");
            if (!file)
            {
                std::cerr << "Corrupted Language Files" << std::endl;
                exit(1);
            }
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        Json::Reader reader;

        if (!reader.parse(buffer.str(), LocaleStrings))
        {
            std::cerr << "Corrupted Language Files" << std::endl;
            exit(1);
        }
    }

    static Json::Value GetNode(std::string key)
    {

        if (key == "")
            return NULL;

        if (LocaleStrings.isNull())
            LoadLanguageJson();

        Json::Value child = LocaleStrings;
        size_t aux = key.find(':');
        std::string token = key.substr(0, aux);
        key.erase(0, aux + 1);
        std::string lastToken = "";

        while (key != "" && token != key)
        {
            if (child[token].isNull())
                return NULL;

            child = child[token];

            lastToken = token;
            aux = key.find(':');
            token = key.substr(0, aux);
            key.erase(0, aux + 1);
        }
        return child[token];
    }

    static std::string Get(std::string key)
    {
        Json::Value child = GetNode(key);

        if (child == NULL || child.isNull())
            return "";

        return child.asString();
    }

    static std::vector<std::string> GetArray(std::string key)
    {
        Json::Value child = GetNode(key);
        std::vector<std::string> aux;

        if (child == NULL || child.isNull())
            return aux;

        int size = child.size();

        for (int i = 0; i < size; i++)
        {   
            std::string value = child[i].asString();
            aux.push_back(value);
        }
        return aux;
    }
}