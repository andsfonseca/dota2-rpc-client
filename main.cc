#include <iostream>
#include <fstream>
#include <filesystem>
#include <drogon/drogon.h>
#include "services/DiscordService.cpp"
#include <regex>

namespace fs = std::filesystem;

void findAndReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while( pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

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
            if (pos1!= std::string::npos)
            {   
                findAndReplaceAll(library_path, "\\\\", "/");
                return library_path + "/steamapps/common/dota 2 beta/game/dota/cfg/gamestate_integration/";
            }
            
        }
        file.close();
    }
    return "";
}

void resolveDota2GameStateIntegration()
{
    bool found = false;

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
        std::cout << cfg_folder; 
    }
}
int main(int argc, char *argv[])
{

    resolveDota2GameStateIntegration();
    return 0;
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

    // Discord Inicialization
    DiscordService *discordService = discordService->getInstance();
    discordService->Initialize();
    discordService->Start();

    discordService->UpdateActivity("Playing All Random", "Rubick (10/0/12)");

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
