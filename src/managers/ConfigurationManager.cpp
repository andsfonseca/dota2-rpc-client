#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <extensions/StringExtensions.h>
#include <persistence/JsonLoader.h>
#include <services/DiscordService.h>
#include <filesystem>
#include <iostream>

Json::Value ConfigurationManager::configurations = static_cast<Json::Value>(NULL);

void ConfigurationManager::load()
{
    JsonLoader::load(JsonLoader::getApplicationFolder() + "/config.json", configurations);
}

std::string ConfigurationManager::getHost()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "HOST");

    if (value == NULL)
    {
        std::cout << "Unable to find configuration file. Using default (host = '127.0.0.1')" << std::endl;
        return "127.0.0.1";
    }

    return value.asString();
}

unsigned int ConfigurationManager::getPort()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "PORT");

    if (value == NULL)
    {
        std::cout << "Unable to find configuration file. Using default (port = 52424)" << std::endl;
        return 52424;
    }

    return value.asUInt();
}

std::string ConfigurationManager::getSteamPath()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "STEAM_FOLDER");

    if (!value.isNull())
        return value.asString();

#ifdef __linux__
    const char* home = std::getenv("HOME");

    if (!home)
        return "";
    
    std::vector<std::string> paths = {
        std::string(home) + "/.local/share/Steam", // Debian/Ubuntu
        std::string(home) + "/.steam/steam", // Old Steam path
        std::string(home) + "/.var/app/com.valvesoftware.Steam/.local/share/Steam", // Flatpak
        std::string(home) + "/snap/steam/common/.local/share/Steam" // Snap
    };
    
    for (const auto& path : paths)
    {
        if (std::filesystem::exists(path))
            return path;
    }
    
    return "";
#elif _WIN32
    HKEY hKey;

    const char* subkeys[] = {
        "SOFTWARE\\WOW6432Node\\Valve\\Steam",
        "SOFTWARE\\Valve\\Steam"
    };

    for (const auto& subkey : subkeys)
    {
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
        {
            char buffer[512];
            DWORD bufferSize = sizeof(buffer);
            DWORD type = 0;

            if (RegQueryValueExA( hKey, "InstallPath", NULL, &type, reinterpret_cast<LPBYTE>(buffer), &bufferSize ) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
                std::string path(buffer);
                StringExtensions::findAndReplaceAll(path, "\\\\", "/");
                return path;
            }
            RegCloseKey(hKey);
        }
    }

    return "C:/Program Files (x86)/Steam";
#else
    return "";
#endif
}

bool ConfigurationManager::setSteamPath(const std::string &path)
{
    configurations["STEAM_FOLDER"] = path;
    return JsonLoader::save(JsonLoader::getApplicationFolder() + "/config.json", configurations);
}

bool ConfigurationManager::showAegis()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:AEGIS");

    if (value == NULL)
        return true;

    return value.asBool();
}

bool ConfigurationManager::showAghanim()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:AGHANIM");

    if (value == NULL)
        return true;

    return value.asBool();
}

bool ConfigurationManager::showGoldAndLastHit()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:GOLD_AND_LAST_HIT");

    if (value == NULL)
        return true;

    return value.asBool();
}

bool ConfigurationManager::showKillDeathAssist()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:KILL_DEATH_ASSIST");

    if (value == NULL)
    {
        return true;
    }

    return value.asBool();
}

bool ConfigurationManager::showSmoke()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:SMOKE");

    if (value == NULL)
        return true;

    return value.asBool();
}

std::string ConfigurationManager::getLocale()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "LOCALE");

    std::string lang("System");

    if (value != NULL)
        lang = value.asString();

    lang = StringExtensions::toLowerCase(lang);

    // Update Settings
    if (lang == "system" && configurations != NULL)
    {
        lang = LanguageManager::getSystemLanguage();
        configurations["LOCALE"] = lang;
    }

    return lang;
}