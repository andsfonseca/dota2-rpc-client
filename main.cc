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

#include <extensions/StringExtensions.h>
#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <managers/SteamManager.h>
#include "third_party/srvlib/Service.h"


void Start()
{
    std::string host = ConfigurationManager::getHost();
    unsigned int port_number = ConfigurationManager::getPort();
    std::string listeningMessage;
    // Web Server Messages
    if (host == "0.0.0.0")
    {
        listeningMessage = LanguageManager::getString("APP:INFO:SERVER_LISTENING_LOCALHOST", LanguageManager::getSystemLanguage());
    }
    else
    {
        listeningMessage = LanguageManager::getString("APP:INFO:SERVER_LISTENING", LanguageManager::getSystemLanguage());
    }

    StringExtensions::findAndReplaceAll(listeningMessage, "{{HOST}}", host);
    StringExtensions::findAndReplaceAll(listeningMessage, "{{PORT}}", std::to_string(port_number));
    std::cout << listeningMessage << "\n";
    std::cout << LanguageManager::getString("APP:INFO:SERVER_HOW_TO_EXIT", LanguageManager::getSystemLanguage()) << "\n";

    // Set HTTP listener address and port
    drogon::app().addListener(host, port_number);

    drogon::app().run();
}

void Stop()
{
    drogon::app().quit();
}

std::thread *threadDrogon;

enum ArgumentOptions
{
    NONE,
    INSTALL
};

ArgumentOptions resolveArgumentOption(std::string input)
{
    if (input == "-i" || input == "--install")
        return INSTALL;
    return NONE;
}

void readArgs(const int argc, const char *argv[])
{
    if (argc > 1)
    {
        switch (resolveArgumentOption(argv[1]))
        {
        case INSTALL:
            SteamManager::onInstall();
            break;
        default:
            break;
        }
    }
}

int main(int argc, const char *argv[])
{
    readArgs(argc, argv);

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
    // if (argc <= 1)
    // {
    //     const char *args[] = {"", "-f"};
    //     return ServiceMain(2, args, svParam);
    // }

    return ServiceMain(argc, argv, svParam);
}
