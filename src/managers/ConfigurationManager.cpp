#include <managers/ConfigurationManager.h>
#include <persistence/JsonLoader.h>

#include <iostream>

Json::Value ConfigurationManager::configurations = NULL;

void ConfigurationManager::load()
{
    JsonLoader::load(JsonLoader::getApplicationFolder() + "/config.json", configurations);
}

std::string ConfigurationManager::getHost()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "HOST");

    if (value == NULL)
    {
        std::cout << "Unable to find configuration file. Using default (host = '127.0.0.1')" << std::endl;
        return "127.0.0.1";
    }

    return value.asString();
}

unsigned int ConfigurationManager::getPort()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "PORT");

    if (value == NULL)
    {
        std::cout << "Unable to find configuration file. Using default (port = 52424)" << std::endl;
        return 52424;
    }

    return value.asUInt();
}

bool ConfigurationManager::showAghanim()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:AGHANIM");

    if (value == NULL)
        return true;

    return value.asBool();
}

bool ConfigurationManager::showGoldAndLastHit()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:GOLD_AND_LAST_HIT");

    if (value == NULL)
        return true;

    return value.asBool();
}

bool ConfigurationManager::showKillDeathAssist()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:KILL_DEATH_ASSIST");

    if (value == NULL)
    {
        return true;
    }

    return value.asBool();
}

bool ConfigurationManager::showSmoke()
{
    if (configurations == NULL)
        load();

    Json::Value value = JsonLoader::getNode(configurations, "DISPLAY_OPTIONS:SMOKE");

    if (value == NULL)
        return true;

    return value.asBool();
}