#include <managers/SteamManager.h>
#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <persistence/TemplateLoader.h>
#include <extensions/StringExtensions.h>

#include <filesystem>
#include <iostream>
#include <regex>

#include <nfd.hpp>

std::string SteamManager::getSteamPath()
{
    std::string path = ConfigurationManager::getSteamPath();

    if (std::filesystem::is_directory(path))
        return path;

    return "";
}

std::string SteamManager::showSelectFolderDialog()
{
    // initialize NFD
    NFD::Guard nfdGuard;

    // auto-freeing memory
    NFD::UniquePath outPath;

    // Show the dialog
    nfdresult_t result = NFD::PickFolder(outPath);
    if (result == NFD_OKAY)
    {
        return outPath.get();
    }
    else
    {
        return "";
    }
}

std::string SteamManager::generateDota2CFG()
{
    std::string dota2_template;
    TemplateLoader::load("gamestate_integration_rpc.cfg", dota2_template);
    StringExtensions::findAndReplaceAll(dota2_template,
                                        {"{{HOST}}", "{{PORT}}"},
                                        {ConfigurationManager::getHost(), std::to_string(ConfigurationManager::getPort())});

    return dota2_template;
}

void SteamManager::showCannotCreateFileWarning()
{
    std::cout << StringExtensions::getValueAsANSIColourCodes(
                     LanguageManager::getString("APP:ERRORS:CFG_NOT_FOUND", LanguageManager::getSystemLanguage()),
                     {ANSIColor::FG_RED})
              << "\n\n"
              << StringExtensions::getValueAsANSIColourCodes(
                     "=========== gamestate_integration_rpc.cfg ================",
                     {ANSIColor::FG_BLACK, ANSIColor::BG_WHITE})
              << "\n"
              << generateDota2CFG()
              << "\n"
              << StringExtensions::getValueAsANSIColourCodes(
                     "==========================================================",
                     {ANSIColor::FG_BLACK, ANSIColor::BG_WHITE})
              << "\n\n";
}

bool SteamManager::findDota2PathUsingVDF(const std::string vdfPath, std::string &dota2path)
{

    std::string data;
    if (!FileLoader::load(vdfPath, data))
        return false;

    std::regex path_regex("\"([^\"]+)\"");

    std::istringstream iss(data);
    std::string line;

    std::string library_path;

    while (std::getline(iss, line))
    {
        auto pos = line.find("path", 0);
        if (pos != std::string::npos)
        {
            library_path = line.substr(pos + 5, line.size());
            std::smatch sm;
            std::regex_search(library_path, sm, path_regex);
            library_path = sm[1];
            continue;
        }

        auto pos1 = line.find("\"570\"", 0);
        if (pos1 != std::string::npos)
        {
            StringExtensions::findAndReplaceAll(library_path, "\\\\", "/");
            dota2path = library_path + "/steamapps/common/dota 2 beta";
            return true;
        }
    }
    return false;
}

void SteamManager::onInstall()
{
    std::cout << LanguageManager::getString("APP:INFO:INSTALLATION_IN_PROGRESS", LanguageManager::getSystemLanguage()) << std::endl;
    std::cout << LanguageManager::getString("APP:INFO:FINDING_STEAM", LanguageManager::getSystemLanguage()) << std::endl;

    std::string steamPath(SteamManager::getSteamPath());

    if (steamPath.empty())
    {
        std::cout << StringExtensions::getValueAsANSIColourCodes(
                         LanguageManager::getString("APP:ERRORS:STEAM_NOT_FOUND", LanguageManager::getSystemLanguage()),
                         {ANSIColor::FG_YELLOW})
                  << std::endl;

        steamPath = showSelectFolderDialog();
        StringExtensions::findAndReplaceAll(steamPath, "\\", "/");

        // Update config.json with new path
        ConfigurationManager::setSteamPath(steamPath);
    }
    else
    {
        std::cout << StringExtensions::getValueAsANSIColourCodes(
                         LanguageManager::getString("APP:SUCCESS:FOUND_STEAM", LanguageManager::getSystemLanguage()),
                         {ANSIColor::FG_GREEN})
                  << std::endl;
    }

    // Cannot put CFG File, send warning
    if (steamPath.empty())
    {
        showCannotCreateFileWarning();
        return;
    }

    // Check folder and create first CFG
    std::string vdfPath = steamPath + "/SteamApps/libraryfolders.vdf";

    std::string dota2Path;

    if (!findDota2PathUsingVDF(vdfPath, dota2Path))
    {
        std::cout << StringExtensions::getValueAsANSIColourCodes(
                         LanguageManager::getString("APP:ERRORS:DOTA_NOT_FOUND", LanguageManager::getSystemLanguage()),
                         {ANSIColor::FG_YELLOW})
                  << std::endl;

        dota2Path = showSelectFolderDialog();
        StringExtensions::findAndReplaceAll(dota2Path, "\\", "/");
    }
    else
    {
        std::cout << StringExtensions::getValueAsANSIColourCodes(
                         LanguageManager::getString("APP:SUCCESS:FOUND_DOTA", LanguageManager::getSystemLanguage()),
                         {ANSIColor::FG_GREEN})
                  << std::endl;
    }

    if (dota2Path.empty())
    {
        showCannotCreateFileWarning();
        return;
    }

    dota2Path = dota2Path + "/game/dota/cfg/gamestate_integration";

    // Check if directory is already exists
    if (!std::filesystem::is_directory(dota2Path) || !std::filesystem::exists(dota2Path))
    {
        try
        {
            std::cout << LanguageManager::getString("APP:INFO:CREATING_FOLDER", LanguageManager::getSystemLanguage())
                      << std::endl;

            std::filesystem::create_directory(dota2Path);

            std::cout << StringExtensions::getValueAsANSIColourCodes(
                             LanguageManager::getString("APP:SUCCESS:CREATED_FOLDER", LanguageManager::getSystemLanguage()),
                             {ANSIColor::FG_GREEN})
                      << std::endl;
        }
        catch (...)
        {
            std::string error = LanguageManager::getString("APP:ERRORS:CREATION_FOLDER_FAILURE", LanguageManager::getSystemLanguage());
            StringExtensions::findAndReplaceAll(error, "{{PATH}}", dota2Path);
            std::cout << StringExtensions::getValueAsANSIColourCodes(error, {ANSIColor::FG_YELLOW}) << std::endl;
            showCannotCreateFileWarning();
            return;
        }
    }

    try
    {
        std::cout << LanguageManager::getString("APP:INFO:CREATING_FILE", LanguageManager::getSystemLanguage())
                  << std::endl;

        FileLoader::save(dota2Path + "/gamestate_integration_rpc.cfg", generateDota2CFG());

        std::cout << StringExtensions::getValueAsANSIColourCodes(
                         LanguageManager::getString("APP:SUCCESS:CREATED_FILE", LanguageManager::getSystemLanguage()),
                         {ANSIColor::FG_GREEN})
                  << std::endl;
    }
    catch (...)
    {
        std::string error = LanguageManager::getString("APP:ERRORS:CREATION_FILE_FAILURE", LanguageManager::getSystemLanguage());
        StringExtensions::findAndReplaceAll(error, "{{PATH}}", dota2Path + "/gamestate_integration_rpc.cfg");
        std::cout << StringExtensions::getValueAsANSIColourCodes(error, {ANSIColor::FG_YELLOW}) << std::endl;
        showCannotCreateFileWarning();
        return;
    }

    std::cout << std::endl
              << StringExtensions::getValueAsANSIColourCodes(
                     LanguageManager::getString("APP:SUCCESS:INSTALL_CFG", LanguageManager::getSystemLanguage()),
                     {ANSIColor::FG_GREEN})
              << std::endl;
}