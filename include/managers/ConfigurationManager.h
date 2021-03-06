#pragma once

#include <string>

#include <json/json.h>

class ConfigurationManager
{
public:
    static std::string getHost();
    static std::string getLocale();
    static unsigned int getPort();
    static std::string getSteamPath();
    static bool setSteamPath(const std::string &path);
    static bool showAegis();
    static bool showAghanim();
    static bool showGoldAndLastHit();
    static bool showKillDeathAssist();
    static bool showSmoke();
    static void load();

protected:
    static Json::Value configurations;
};