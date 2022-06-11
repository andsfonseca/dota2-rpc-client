#pragma once

#include <string>

#include <json/json.h>

class ConfigurationManager
{
public:
    static std::string getHost();
    static unsigned int getPort();
    static bool showAghanim();
    static bool showGoldAndLastHit();
    static bool showKillDeathAssist();
    static bool showSmoke();
    static void load();

protected:
    static Json::Value configurations;
};