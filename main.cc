#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include "third_party/SrvLib/Service.h"
#pragma comment(lib, "SrvLib.lib")
#else
#include <codecvt>
#include <locale>
#include <syslog.h>
#include <unistd.h>
#endif

#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <managers/SteamManager.h>
#include <managers/WebServerManager.h>

enum ArgumentOptions
{
    NONE,
    INSTALL,
};

ArgumentOptions resolveArgumentOption(const std::string input)
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
#if defined(_WIN32) || defined(_WIN64)
    SrvParam svParam;
#if defined(_WIN32) || defined(_WIN64)
    svParam.szDspName = L"Dota 2 RPC Client Service"; // Servicename in Service control manager of windows
    svParam.szDescrip = L"Dota 2 RPC Client Service"; // Description in Service control manager of windows
#endif
    svParam.szSrvName = L"dota2rpc"; // Service name (service id)

    svParam.fnStartCallBack = WebServerManager::start;
    svParam.fnStopCallBack = WebServerManager::stop;

    svParam.fnSignalCallBack = []() {
    };

    // No option, run sync
    // if (argc <= 1)
    // {
    //     const char *args[] = {"", "-f"};
    //     return ServiceMain(2, args, svParam);
    // }
    
    return ServiceMain(argc, argv, svParam);
#else
    WebServerManager::onStart();
    return 0;
#endif
}
