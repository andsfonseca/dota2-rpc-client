#include "../templates/Templates.cpp"
#include <json/json.h>

namespace LocalizedStrings
{
    static Json::Value LocaleStrings;

    static std::string GetLocale()
    {
#if defined(_WIN32) || defined(_WIN64)
        LCID lcid = GetThreadLocale();
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
        if (LCIDToLocaleName(lcid, localeName, LOCALE_NAME_MAX_LENGTH, 0) == 0)
            return "en-us";

        return Extensions::ToLowerCase(Extensions::ConvertWideToUtf8(localeName));
#else
        std::string localeName1 = std::setlocale(LC_ALL, "");
        std::string localeP = localeName1.substr(0, localeName1.find('.'));
        return Extensions::ToLowerCase(localeP);
#endif
    }

    static void LoadLanguageJson()
    {
        const std::string LANGUAGE_FOLDER = "/lang/";
        const std::string LANGUAGE_DEFAULT = "en-us";

        std::string folder = Templates::GetExecutableFolder();
        std::string localeName = GetLocale();

        std::string fileAsString;

        if(!Templates::LoadFile(folder + LANGUAGE_FOLDER + localeName + ".json", fileAsString)){
            if(!Templates::LoadFile(folder + LANGUAGE_FOLDER + LANGUAGE_DEFAULT + ".json", fileAsString)){
                std::cerr << "Corrupted Language Files" << std::endl;
                exit(1);
            }
        }

        Json::Reader reader;

        if (!reader.parse(fileAsString, LocaleStrings))
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