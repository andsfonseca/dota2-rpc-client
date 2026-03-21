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

void readArgs(const int argc, char *argv[])
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

int main(int argc, char *argv[])
{
#if defined(_WIN32) || defined(_WIN64)
    SrvParam svParam;
    svParam.szDspName = L"Dota 2 RPC Client Service";
    svParam.szDescribe = L"Dota 2 RPC Client Service";
    svParam.szSrvName = L"dota2rpc";

    svParam.fnStartCallBack = WebServerManager::start;
    svParam.fnStopCallBack = WebServerManager::stop;

    svParam.fnSignalCallBack = []() {};

    return ServiceMain(argc, argv, svParam);
#else
    WebServerManager::onStart();
    return 0;
#endif
}
