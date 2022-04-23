#include <regex>
#include <thread>
#include <drogon/drogon.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#pragma comment(lib, "SrvLib.lib")
#else
#include <codecvt>
#include <locale>
#include <syslog.h>
#include <unistd.h>
#endif

#include "third_party/srvlib/Service.h"

#include "src/templates/CFGJSON.hpp"
#include "src/localization/LocalizedStrings.cpp"


namespace fs = std::filesystem;

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
                Extensions::FindAndReplaceAll(library_path, "\\\\", "/");
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
    Extensions::FindAndReplaceAll(dota2_template, "{{host}}", host);
    Extensions::FindAndReplaceAll(dota2_template, "{{port}}", std::to_string(port));

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
            outfile << cfg_source << std::endl;
        }
        else
        {
            found = false;
        }
    }

    if (!found)
    {
        std::cout <<  LocalizedStrings::Get("APP:ERRORS:CFG_NOT_FOUND") <<"\n\n";
        std::cout << "=========== gamestate_integration_rpc.cfg ================\n";
        std::cout << cfg_source << "\n";
        std::cout << "==========================================================\n\n";
    }
}

void Start()
{
    std::string host = "127.0.0.1";
    int port_number = 52424;
    resolveDota2GameStateIntegration(host, port_number);
    std::string listeningMessage;
    // Web Server Messages
    if (host == "0.0.0.0")
    {
        listeningMessage = LocalizedStrings::Get("APP:INFO:SERVER_LISTENING_LOCALHOST");
    }
    else
    {
        listeningMessage = LocalizedStrings::Get("APP:INFO:SERVER_LISTENING");
    }

    Extensions::FindAndReplaceAll(listeningMessage, "{{HOST}}", host);
    Extensions::FindAndReplaceAll(listeningMessage, "{{PORT}}", std::to_string(port_number));
    std::cout << listeningMessage << "\n";
    std::cout << LocalizedStrings::Get("APP:INFO:SERVER_HOW_TO_EXIT") <<"\n";

    // Set HTTP listener address and port
    drogon::app().addListener(host, port_number);

    drogon::app().run();
}

void Stop()
{
    drogon::app().quit();
}

std::thread *threadDrogon;

int main(int argc, const char *argv[])
{
    SrvParam svParam;
#if defined(_WIN32) || defined(_WIN64)
    svParam.szDspName = L"Dota 2 RPC Client Service"; // Servicename in Service control manager of windows
    svParam.szDescrip = L"Dota 2 RPC Client Service"; // Description in Service control manager of windows
#endif
    svParam.szSrvName = L"dota2rpc"; // Service name (service id)

    svParam.fnStartCallBack = []()
    {
        threadDrogon = new std::thread(&Start);
    };
    svParam.fnStopCallBack = []()
    {
        Stop();
        threadDrogon->join();
    };
    svParam.fnSignalCallBack = []() {
    };

    // No option, run sync
    if (argc <= 1)
    {
        const char *args[] = {"", "-f"};
        return ServiceMain(2, args, svParam);
    }

    return ServiceMain(argc, argv, svParam);
}
