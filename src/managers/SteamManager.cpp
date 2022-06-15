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

    return "";
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
    std::cout << LanguageManager::getString("APP:ERRORS:CFG_NOT_FOUND", LanguageManager::getSystemLanguage()) << "\n\n";
    std::cout << "=========== gamestate_integration_rpc.cfg ================\n";
    std::cout << generateDota2CFG() << "\n";
    std::cout << "==========================================================\n\n";
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

        auto pos1 = line.find("570", 0);
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
    std::cout << "Installing DotA 2 RPC..." << std::endl;
    std::cout << "Finding Steam folder..." << std::endl;

    std::string steamPath(SteamManager::getSteamPath());

    if (steamPath.empty())
    {
        std::cout << "Unable to find steam folder, please set the directory..." << std::endl;

        steamPath = showSelectFolderDialog();
        StringExtensions::findAndReplaceAll(steamPath, "\\", "/");

        // Update config.json with new path
        ConfigurationManager::setSteamPath(steamPath);
    }

    // Cannot put CFG File, send warning
    if (steamPath == "")
    {
        showCannotCreateFileWarning();
        return;
    }

    // Check folder and create first CFG
    std::string vdfPath = steamPath + "/SteamApps/libraryfolders.vdf";

    std::string dota2Path;
    
    if(!findDota2PathUsingVDF(vdfPath, dota2Path))
    {
        std::cout << "Unable to find 'dota 2 beta' folder using steam library, please set the directory..." << std::endl;

        dota2Path = showSelectFolderDialog();
        StringExtensions::findAndReplaceAll(dota2Path, "\\", "/");
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
        std::filesystem::create_directory(dota2Path);
    }

    FileLoader::save(dota2Path + "/gamestate_integration_rpc.cfg", generateDota2CFG());
    
    std::cout << "Cfg file added successfully. The changes will take effect the next time you launch the game." << std::endl;
}