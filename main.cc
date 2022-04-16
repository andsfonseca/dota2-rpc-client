#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <drogon/drogon.h>

#include "services/DiscordService.cpp"

#include "utils/CFGJSON.hpp"
#include "utils/StringExtensions.cpp"

namespace fs = std::filesystem;

enum ArgumentOptions
{
    NONE,
    HOST,
    PORT,
};

ArgumentOptions resolveArgumentOption(std::string input)
{
    if (input == "-h" || input == "--host")
        return HOST;
    if (input == "-p" || input == "--port")
        return PORT;
    return NONE;
}

std::string getDota2CFGPathLocationFromVDFFile(std::string path)
{
    std::regex path_regex("\"([^\"]+)\"");

    std::ifstream file(path);

    if (file.is_open())
    {
        std::string line;
        std::string library_path;

        while (std::getline(file, line))
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
                Extensions::findAndReplaceAll(library_path, "\\\\", "/");
                return library_path + "/steamapps/common/dota 2 beta/game/dota/cfg/gamestate_integration";
            }
        }
        file.close();
    }
    return "";
}

std::string makeDota2CFGFile(std::string host, int port)
{
    std::string dota2_template(DOTA2_CFG_TEMPLATE);
    Extensions::findAndReplaceAll(dota2_template, "{{host}}", host);
    Extensions::findAndReplaceAll(dota2_template, "{{port}}", std::to_string(port));

    return dota2_template;
}

void resolveDota2GameStateIntegration(std::string host, int port)
{
    bool found = false;

    std::string cfg_source = makeDota2CFGFile(host, port);

#ifdef __linux__
    std::string library_vdf_path = "~/.steam/steam/SteamApps";
    if (fs::is_directory(library_vdf_path))
    {
        library_vdf_path + "/libraryfolders.vdf";
        found = true;
    }
#elif _WIN32
    std::string library_vdf_path = "C:/Program Files (x86)/Steam/steamapps";
    if (fs::is_directory(library_vdf_path))
    {
        library_vdf_path = library_vdf_path + "/libraryfolders.vdf";
        found = true;
    }
#else
    std::string library_vdf_path = "~/Library/Application Support/Steam/steamapps";
    if (fs::is_directory(library_vdf_path))
    {
        library_vdf_path + "/libraryfolders.vdf";
        found = true;
    }
#endif

    if (found)
    {

        std::string cfg_folder = getDota2CFGPathLocationFromVDFFile(library_vdf_path);

        if (!cfg_folder.empty())
        {
            // Check if directory is already exists
            if (!fs::is_directory(cfg_folder) || !fs::exists(cfg_folder))
            {
                fs::create_directory(cfg_folder);
            }

            std::ofstream outfile(cfg_folder + "/gamestate_integration_rpc.cfg");
            outfile << cfg_source << std::endl;;
        }
        else
        {
            found = false;
        }
    }

    if (!found)
    {
        std::cout << "Unable to find DotA 2 files!\n";
        std::cout << "Please create the following file with the name \"gamestate_integration_rpc.cfg\" and insert it\n";
        std::cout << "into the folder \"{{STEAM_LIBRARY_PATH}}/dota 2 beta/game/dota/cfg/gamestate_integration/\".\n\n";
        std::cout << "=========== gamestate_integration_rpc.cfg ================\n";
        std::cout << cfg_source << "\n";
        std::cout << "==========================================================\n\n";
    }
}

int main(int argc, char *argv[])
{
    std::string host = "127.0.0.1";
    int port_number = 52424;

    if (argc > 1)
    {
        for (int i = 1; i < argc;)
        {
            switch (resolveArgumentOption(argv[i]))
            {
            case HOST:
                host = argv[i + 1];
                i = i + 2;
                break;
            case PORT:
                port_number = std::stoi(argv[i + 1]);
                i = i + 2;
                break;
            default:
                break;
            }
        }
    }

    resolveDota2GameStateIntegration(host, port_number);
    // Discord Inicialization
    DiscordService *discordService = discordService->getInstance();
    if(!discordService->Initialize())
        return 0;
    discordService->Start();

    // Set HTTP listener address and port
    drogon::app().addListener(host, port_number);

    // Web Server Messages
    if (host == "0.0.0.0")
    {
        std::cout << "Listening at http://localhost:" << port_number << " and http://" << host << ":" << port_number << ".\n";
    }
    else
    {
        std::cout << "Listening at http://" << host << ":" << port_number << ".\n";
    }
    std::cout << "Press Ctrl+C to exit.\n";

    drogon::app().run();

    discordService->Stop();
    return 0;
}
