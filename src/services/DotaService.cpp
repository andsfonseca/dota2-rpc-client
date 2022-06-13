#include <services/DotaService.h>

#include <extensions/StringExtensions.h>
#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <persistence/FileLoader.h>
#include <services/DiscordService.h>

#include <iostream>
#include <regex>
#include <chrono>
// #include <map>

DotaService *DotaService::instance = nullptr;

DotaService::DotaService() {}

PlayerStatus DotaService::getPlayerStatus(Json::Value data)
{
    if (data["player"].isNull())
        return PlayerStatus::STAND_BY;

    if (!data["player"]["team2"].isNull() && !data["player"]["team3"].isNull())
        return PlayerStatus::WATCHING;

    if (data["player"]["activity"].isNull())
        return PlayerStatus::STAND_BY;

    if (!data["player"]["team_name"].isNull() && data["player"]["team_name"].asString() == "spectator")
        return PlayerStatus::COACHING;

    std::string activity = data["player"]["activity"].asString();

    if (activity != "playing")
    {
        std::string message = LanguageManager::getString("APP:ERRORS:UNKNOWN_ACTIVITY", LanguageManager::getSystemLanguage());
        StringExtensions::findAndReplaceAll(message, "{{ACTIVITY}}", activity);
        std::cout << message << "\n";
    }

    return PlayerStatus::PLAYING;
}

GameState DotaService::getCurrentGameState(Json::Value data)
{
    if (data["map"].isNull())
        return GameState::NONE;

    if (data["map"]["game_state"].isNull())
        return GameState::NONE;

    std::string gamestate = data["map"]["game_state"].asString();

    if (gamestate == "DOTA_GAMERULES_STATE_INIT" ||
        gamestate == "DOTA_GAMERULES_STATE_WAIT_FOR_PLAYERS_TO_LOAD" ||
        gamestate == "DOTA_GAMERULES_STATE_HERO_SELECTION")
        return GameState::HERO_SELECTION;
    else if (gamestate == "DOTA_GAMERULES_STATE_STRATEGY_TIME" ||
             gamestate == "DOTA_GAMERULES_STATE_WAIT_FOR_MAP_TO_LOAD" ||
             gamestate == "DOTA_GAMERULES_STATE_TEAM_SHOWCASE")
        return GameState::STRATEGY_TIME;
    else if (gamestate == "DOTA_GAMERULES_STATE_PRE_GAME")
        return GameState::PRE_GAME;
    else if (gamestate == "DOTA_GAMERULES_STATE_GAME_IN_PROGRESS")
        return GameState::GAME;
    else if (gamestate == "DOTA_GAMERULES_STATE_POST_GAME")
        return GameState::NONE;
    else
    {
        std::string message = LanguageManager::getString("APP:ERRORS:UNKNOWN_GAMESTATE", LanguageManager::getSystemLanguage());
        StringExtensions::findAndReplaceAll(message, "{{GAMESTATE}}", gamestate);
        std::cout << message << "\n";
        return GameState::NONE;
    }
}

int DotaService::getGameTimeElapsed(Json::Value data)
{
    if (data["map"].isNull())
        return 0;

    if (data["map"]["clock_time"].isNull())
        return 0;

    int time = data["map"]["clock_time"].asInt();

    return time;
}

int DotaService::getMatchTimeElapsed(Json::Value data)
{
    if (data["map"].isNull())
        return 0;

    if (data["map"]["game_time"].isNull())
        return 0;

    int time = data["map"]["game_time"].asInt();

    return time;
}

std::string DotaService::getHeroName(Json::Value data)
{
    if (data["hero"].isNull())
        return "";

    if (data["hero"]["name"].isNull())
        return "";

    std::string name = data["hero"]["name"].asString();

    // Weareables
    if (data["wearables"].isNull())
        return name;

    for (int i = 0;; i++)
    {
        std::string key = "wearable" + std::to_string(i);

        if (data["wearables"][key].isNull())
            break;

        int itemId = data["wearables"][key].asInt();

        // Personas First
        if ((name == "npc_dota_hero_antimage" && itemId == 13783) ||
            (name == "npc_dota_hero_dragon_knight" && itemId == 18113) ||
            (name == "npc_dota_hero_mirana" && itemId == 18178) ||
            (name == "npc_dota_hero_invoker" && itemId == 13042))
            return name + "_2";
        if (name == "npc_dota_hero_pudge" && itemId == 13786)
            return name + "_4";

        // Arcanas and Prestige Items
        if ((name == "npc_dota_hero_axe" && itemId == 12964) ||
            (name == "npc_dota_hero_crystal_maiden" && itemId == 7385) ||
            (name == "npc_dota_hero_drow_ranger" && itemId == 19090) ||
            (name == "npc_dota_hero_earthshaker" && itemId == 12692) ||
            (name == "npc_dota_hero_wisp" && itemId == 9235) ||
            (name == "npc_dota_hero_juggernaut" && itemId == 9059) ||
            (name == "npc_dota_hero_legion_commander" && itemId == 5810) ||
            (name == "npc_dota_hero_lina" && itemId == 4794) ||
            (name == "npc_dota_hero_monkey_king" && itemId == 9050) ||
            (name == "npc_dota_hero_ogre_magi" && itemId == 13670) ||
            (name == "npc_dota_hero_phantom_assassin" && itemId == 7247) ||
            (name == "npc_dota_hero_pudge" && itemId == 7756) ||
            (name == "npc_dota_hero_queenofpain" && itemId == 12930) ||
            (name == "npc_dota_hero_rubick" && itemId == 12451) ||
            (name == "npc_dota_hero_nevermore" && itemId == 6996) ||
            (name == "npc_dota_hero_spectre" && itemId == 9662) ||
            (name == "npc_dota_hero_terrorblade" && itemId == 5957) ||
            (name == "npc_dota_hero_windrunner" && itemId == 13806) ||
            (name == "npc_dota_hero_skeleton_king" && itemId == 13456))
        {
            // Second Style
            std::string style = "style" + std::to_string(i);
            if (!data["wearables"][style].isNull())
            {
                int numberStyle = data["wearables"][style].asInt();
                if (numberStyle == 1)
                    return name + "_3";
            }

            return name + "_2";
        }
    }

    return name;
}

std::string DotaService::resolveHeroName(std::string key)
{
    std::string name = LanguageManager::getString("DOTA_2:HEROES:" + key, LanguageManager::getSystemLanguage());

    if (name == "")
        return key;

    return name;
}

int DotaService::getHeroLevel(Json::Value data)
{
    if (data["hero"].isNull())
        return 1;

    if (data["hero"]["level"].isNull())
        return 1;

    int level = data["hero"]["level"].asInt();

    return level;
}

void DotaService::getKillDeathAssists(Json::Value data, int &kill, int &death, int &assist)
{
    kill = 0;
    death = 0;
    assist = 0;

    if (data["player"].isNull())
        return;

    if (data["player"]["kills"].isNull() || data["player"]["assists"].isNull() || data["player"]["deaths"].isNull())
        return;

    kill = data["player"]["kills"].asInt();
    death = data["player"]["deaths"].asInt();
    assist = data["player"]["assists"].asInt();
}

long long DotaService::getMatchId(Json::Value data)
{
    if (data["map"].isNull())
        return -1;

    if (data["map"]["matchid"].isNull())
        return -1;

    std::string matchId = data["map"]["matchid"].asString();

    std::stringstream in;
    in << matchId;
    long long i;
    in >> i;

    return i;
}

std::string DotaService::getNeutralNameBasedOnMatchId(long long i)
{
    auto neutralNames = LanguageManager::getArray("DOTA_2:NEUTRALS", LanguageManager::getSystemLanguage());
    size_t size = neutralNames.size();
    return neutralNames[i % size];
}

std::string DotaService::getNetWorth(Json::Value data)
{

    int radiant = 0;
    int dire = 0;

    if (data["player"].isNull())
        return LanguageManager::getString("APP:ACTIVITY_MESSAGES:MATCH:NET_WORTH", LanguageManager::getSystemLanguage());

    if (data["player"]["team2"].isNull() || data["player"]["team3"].isNull())
        return LanguageManager::getString("APP:ACTIVITY_MESSAGES:MATCH:NET_WORTH", LanguageManager::getSystemLanguage());

    int i = 0;

    for (;; i++)
    {
        std::string player = "player" + std::to_string(i);
        if (data["player"]["team2"][player].isNull())
            break;

        if (!data["player"]["team2"][player]["net_worth"].isNull())
        {
            int count = data["player"]["team2"][player]["net_worth"].asInt();
            radiant += count;
        }
    }

    for (;; i++)
    {
        std::string player = "player" + std::to_string(i);
        if (data["player"]["team3"][player].isNull())
            break;

        if (!data["player"]["team3"][player]["net_worth"].isNull())
        {
            int count = data["player"]["team3"][player]["net_worth"].asInt();
            dire += count;
        }
    }

    std::string status;
    int value = 0;
    if (radiant > dire)
    {
        value = (radiant - dire) / 1000;
        status = LanguageManager::getString("APP:ACTIVITY_MESSAGES:MATCH:NET_WORTH_RADIANT", LanguageManager::getSystemLanguage());
    }
    else if (dire > radiant)
    {
        value = (dire - radiant) / 1000;
        status = LanguageManager::getString("APP:ACTIVITY_MESSAGES:MATCH:NET_WORTH_DIRE", LanguageManager::getSystemLanguage());
    }
    else
        status = LanguageManager::getString("APP:ACTIVITY_MESSAGES:MATCH:NET_WORTH", LanguageManager::getSystemLanguage());

    StringExtensions::findAndReplaceAll(status, "{{VALUE}}", std::to_string(value));

    return status;
}

bool DotaService::playerHasAegis(Json::Value data)
{
    if (data["items"].isNull())
        return false;

    for (int i = 0;; i++)
    {
        std::string key = "slot" + std::to_string(i);

        if (data["items"][key].isNull())
            break;

        if (!data["items"][key]["name"].isNull())
        {
            if (data["items"][key]["name"].asString() == "item_aegis")
                return true;
        }
    }

    return false;
}

ItemStatusEffect DotaService::getItemStatusEffect(Json::Value data)
{
    if (data["hero"].isNull())
        return ItemStatusEffect::WITHOUT_ITEMS;

    if (data["hero"]["smoked"].isNull() ||
        data["hero"]["aghanims_shard"].isNull() ||
        data["hero"]["aghanims_scepter"].isNull())
        return ItemStatusEffect::WITHOUT_ITEMS;

    bool smoked = data["hero"]["smoked"].asBool();

    if (ConfigurationManager::showSmoke() && smoked)
        return ItemStatusEffect::SMOKE;

    if (ConfigurationManager::showAegis() && playerHasAegis(data))
        return ItemStatusEffect::AEGIS;

    bool aghanimsShard = data["hero"]["aghanims_shard"].asBool();
    bool aghanimsScepter = data["hero"]["aghanims_scepter"].asBool();

    if (ConfigurationManager::showAghanim())
    {
        if (aghanimsShard && aghanimsScepter)
            return ItemStatusEffect::SCEPTER_AND_SHARD;
        else if (aghanimsScepter)
            return ItemStatusEffect::SCEPTER;
        else if (aghanimsShard)
            return ItemStatusEffect::SHARD;
    }

    return ItemStatusEffect::WITHOUT_ITEMS;
}

void DotaService::getPlayerHits(Json::Value data, int &lastHits, int &denies)
{
    lastHits = 0;
    denies = 0;

    if (data["player"].isNull())
        return;

    if (data["player"]["last_hits"].isNull() || data["player"]["denies"].isNull())
        return;

    lastHits = data["player"]["last_hits"].asInt();
    denies = data["player"]["denies"].asInt();
}

int DotaService::getPlayerGold(Json::Value data)
{
    if (data["player"].isNull())
        return 0;

    if (data["player"]["gold"].isNull())
        return 0;

    return data["player"]["gold"].asInt();
}

std::string DotaService::getWorkshopMapName(const std::string path)
{
    if (WorkshopMapsCache.count(path))
        return WorkshopMapsCache.find(path)->second;

    const size_t last_slash_idx = path.rfind('/');

    if (std::string::npos == last_slash_idx)
        return "";

    std::string directory = path.substr(0, last_slash_idx + 1);
    std::string file;

    if (FileLoader::load(directory + "publish_data.txt", file))
    {
        std::regex title_regex("\"(title)\"\\s+\"((\\\"|[^\"])*)\"");

        std::istringstream f(file);
        std::string line;

        while (std::getline(f, line))
        {
            std::smatch sm;
            std::regex_search(line, sm, title_regex);
            if (sm[2] != "")
            {
                WorkshopMapsCache.insert({path, sm[2]});
                return sm[2];
            }
        }
    }

    return "";
}

std::string DotaService::getMapName(Json::Value data)
{
    if (data["map"].isNull())
        return "";

    if (data["map"]["customgamename"].isNull())
        return "";

    std::string mappath = data["map"]["customgamename"].asString();

    if (mappath == "")
        return "";

    // Try find in custom map string
    StringExtensions::findAndReplaceAll(mappath, "\\", "/");
    std::string mapName(mappath.substr(mappath.rfind("/") + 1));

    std::string customMapName = LanguageManager::getString("DOTA_2:CUSTOM_MAP:" + mapName, LanguageManager::getSystemLanguage());

    if (customMapName != "")
        return customMapName;

    // Try check in Workshop files
    mapName = getWorkshopMapName(mappath);

    if (mapName == "")
        return mapName;

    customMapName = LanguageManager::getString("DOTA_2:CUSTOM_MAP:WORKSHOP", LanguageManager::getSystemLanguage());

    StringExtensions::findAndReplaceAll(customMapName, "{{MAPNAME}}", mapName);
    return customMapName;
}

void DotaService::fixGameTimeIfNecessary(int64_t &matchTime)
{
    const int64_t TOL = 2;
    if ((matchTime - TOL) <= currentMatchTime && currentMatchTime <= (matchTime + TOL))
        matchTime = currentMatchTime;
    else
        currentMatchTime = matchTime;
}

DotaService *DotaService::getInstance()
{
    if (!instance)
        instance = new DotaService;
    return instance;
}

void DotaService::interpretJson(Json::Value data)
{

    // Retrieve time at start;
    auto now = std::chrono::system_clock::now();

    DiscordService *discordService = discordService->getInstance();
    PlayerStatus playerStatus = getPlayerStatus(data);

    if (playerStatus == PlayerStatus::STAND_BY)
    {
        discordService->cleanActivity();
        currentMatchTime = 0;
        return;
    }

    // Retrive Match Informations
    GameState gameState = getCurrentGameState(data);
    int gameTime = getGameTimeElapsed(data);
    int matchTime = getMatchTimeElapsed(data);

    discord::Activity activity{};

    switch (playerStatus)
    {
    case PlayerStatus::PLAYING:
    {
        // Type Section
        activity.SetType(discord::ActivityType::Playing);

        switch (gameState)
        {
        case GameState::HERO_SELECTION:
        {
            // Details Section
            std::string neutralImageKey =
                LanguageManager::getArray("DOTA_2:NEUTRALS", LanguageManager::getSystemLanguage())[3];
            activity.SetDetails(const_cast<char *>(LanguageManager::getString(
                                                       "APP:ACTIVITY_MESSAGES:GAMESTATES:HERO_SELECTION_ALT",
                                                       LanguageManager::getSystemLanguage())
                                                       .c_str()));

            // State Section
            activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                     "APP:ACTIVITY_MESSAGES:GAMESTATES:HERO_SELECTION",
                                                     LanguageManager::getSystemLanguage())
                                                     .c_str()));

            // Big Image
            activity.GetAssets().SetLargeImage(neutralImageKey.c_str());
            break;
        }
        case GameState::STRATEGY_TIME:
        {
            // Details Section
            std::string heroKey = getHeroName(data);
            std::string details = LanguageManager::getString(
                "APP:ACTIVITY_MESSAGES:PLAYER:PLAYING_AS_HERO",
                LanguageManager::getSystemLanguage());

            StringExtensions::findAndReplaceAll(details, "{{NAME}}", resolveHeroName(heroKey));

            long long matchId = getMatchId(data);

            // BotMatch
            if (matchId == 0)
            {
                std::string botPrefix = LanguageManager::getString(
                    "APP:ACTIVITY_MESSAGES:MATCH:BOT_PREFIX",
                    LanguageManager::getSystemLanguage());

                StringExtensions::findAndReplaceAll(botPrefix,
                                                    "{{PLAYING_AS_HERO | PLAYING_AS_HERO_WITH_LEVEL}}",
                                                    details);

                details = botPrefix;
            }

            activity.SetDetails(const_cast<char *>(details.c_str()));

            // State Section
            activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                     "APP:ACTIVITY_MESSAGES:GAMESTATES:STRATEGY_TIME",
                                                     LanguageManager::getSystemLanguage())
                                                     .c_str()));

            // Big Image
            activity.GetAssets().SetLargeImage(heroKey.c_str());
            break;
        }
        case GameState::PRE_GAME:
        case GameState::GAME:
        {
            // Details Section
            int level = getHeroLevel(data);
            std::string heroKey = getHeroName(data);

            std::string details = LanguageManager::getString(
                "APP:ACTIVITY_MESSAGES:PLAYER:PLAYING_AS_HERO_WITH_LEVEL",
                LanguageManager::getSystemLanguage());
            StringExtensions::findAndReplaceAll(details,
                                                {"{{NAME}}", "{{LEVEL}}"},
                                                {resolveHeroName(heroKey), std::to_string(level)});

            long long matchId = getMatchId(data);

            // BotMatch
            if (matchId == 0)
            {
                std::string botPrefix = LanguageManager::getString(
                    "APP:ACTIVITY_MESSAGES:MATCH:BOT_PREFIX",
                    LanguageManager::getSystemLanguage());

                StringExtensions::findAndReplaceAll(botPrefix,
                                                    "{{PLAYING_AS_HERO | PLAYING_AS_HERO_WITH_LEVEL}}",
                                                    details);

                details = botPrefix;
            }

            activity.SetDetails(const_cast<char *>(details.c_str()));

            // State Section
            std::string state = getMapName(data);

            // If no map was found it means you are playing vanilla
            if (state == "")
            {
                // If KDA is enable
                if (ConfigurationManager::showKillDeathAssist())
                {
                    int kill, death, assist;
                    getKillDeathAssists(data, kill, death, assist);

                    state = LanguageManager::getString("APP:ACTIVITY_MESSAGES:PLAYER:KDA",
                                                       LanguageManager::getSystemLanguage());
                    StringExtensions::findAndReplaceAll(state, {"{{KILL}}", "{{DEATH}}", "{{ASSIST}}"},
                                                        {std::to_string(kill), std::to_string(death), std::to_string(assist)});
                }
                // Else, show game state
                else
                {
                    if (gameState == GameState::PRE_GAME)
                        state = LanguageManager::getString(
                            "APP:ACTIVITY_MESSAGES:GAMESTATES:PRE_GAME",
                            LanguageManager::getSystemLanguage());
                    else
                        state = LanguageManager::getString(
                            "APP:ACTIVITY_MESSAGES:GAMESTATES:GAME",
                            LanguageManager::getSystemLanguage());
                }
            }

            activity.SetState(const_cast<char *>(state.c_str()));

            // Time Section
            now += std::chrono::seconds(-gameTime);
            int64_t time0InMatch =
                std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

            fixGameTimeIfNecessary(time0InMatch);
            if (gameState == GameState::PRE_GAME)
                activity.GetTimestamps().SetEnd(DiscordTimestamp(time0InMatch));
            else
                activity.GetTimestamps().SetStart(DiscordTimestamp(time0InMatch));

            // Big Image
            if (ConfigurationManager::showGoldAndLastHit())
            {
                int lastHits, denies;
                getPlayerHits(data, lastHits, denies);

                std::string bigImageDetails = LanguageManager::getString(
                    "APP:ACTIVITY_MESSAGES:PLAYER:GOLD_LH_DN",
                    LanguageManager::getSystemLanguage());

                StringExtensions::findAndReplaceAll(bigImageDetails,
                                                    {"{{GOLD}}", "{{LH}}", "{{DN}}"},
                                                    {std::to_string(getPlayerGold(data)), std::to_string(lastHits), std::to_string(denies)});

                activity.GetAssets().SetLargeText(bigImageDetails.c_str());
            }
            activity.GetAssets().SetLargeImage(heroKey.c_str());

            // Small Image
            ItemStatusEffect effect = getItemStatusEffect(data);

            switch (effect)
            {
            case ItemStatusEffect::SMOKE:
                activity.GetAssets().SetSmallImage("smoke_of_deceit");
                activity.GetAssets().SetSmallText(LanguageManager::getString(
                                                      "APP:ACTIVITY_MESSAGES:PLAYER:SMOKED",
                                                      LanguageManager::getSystemLanguage())
                                                      .c_str());
                break;
            case ItemStatusEffect::AEGIS:
                activity.GetAssets().SetSmallImage("aegis_of_the_immortal");
                activity.GetAssets().SetSmallText(LanguageManager::getString(
                                                      "APP:ACTIVITY_MESSAGES:PLAYER:AEGIS",
                                                      LanguageManager::getSystemLanguage())
                                                      .c_str());
                break;
            case ItemStatusEffect::SCEPTER_AND_SHARD:
                activity.GetAssets().SetSmallImage("aghanims_scepter_2");
                activity.GetAssets().SetSmallText(LanguageManager::getString(
                                                      "APP:ACTIVITY_MESSAGES:PLAYER:HAS_SCEPTER_AND_SHARD",
                                                      LanguageManager::getSystemLanguage())
                                                      .c_str());
                break;
            case ItemStatusEffect::SCEPTER:
                activity.GetAssets().SetSmallImage("aghanims_scepter");
                activity.GetAssets().SetSmallText(LanguageManager::getString(
                                                      "APP:ACTIVITY_MESSAGES:PLAYER:HAS_SCEPTER",
                                                      LanguageManager::getSystemLanguage())
                                                      .c_str());
                break;
            case ItemStatusEffect::SHARD:
                activity.GetAssets().SetSmallImage("aghanims_shard");
                activity.GetAssets().SetSmallText(LanguageManager::getString(
                                                      "APP:ACTIVITY_MESSAGES:PLAYER:HAS_SHARD",
                                                      LanguageManager::getSystemLanguage())
                                                      .c_str());
                break;
            case ItemStatusEffect::WITHOUT_ITEMS:
            default:
                break;
            }

            break;
        }
        case GameState::NONE:
        default:
            return;
        }
        break;
    }
    case PlayerStatus::WATCHING:
    case PlayerStatus::COACHING:
    {
        // Type Section
        activity.SetType(discord::ActivityType::Watching);

        // Details Section and Big Image Section
        if (playerStatus == PlayerStatus::WATCHING)
        {
            activity.SetDetails(const_cast<char *>(LanguageManager::getString(
                                                       "APP:ACTIVITY_MESSAGES:SPECTATOR:WATCH",
                                                       LanguageManager::getSystemLanguage())
                                                       .c_str()));
            activity.GetAssets().SetLargeImage(const_cast<char *>("watching_default"));
        }
        else
        {
            activity.SetDetails(const_cast<char *>(LanguageManager::getString(
                                                       "APP:ACTIVITY_MESSAGES:SPECTATOR:COACH",
                                                       LanguageManager::getSystemLanguage())
                                                       .c_str()));
            activity.GetAssets().SetLargeImage(const_cast<char *>("coaching_default"));
        }

        switch (gameState)
        {
        case GameState::HERO_SELECTION:
            // State Section
            activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                     "APP:ACTIVITY_MESSAGES:GAMESTATES:HERO_SELECTION",
                                                     LanguageManager::getSystemLanguage())
                                                     .c_str()));
            break;
        case GameState::STRATEGY_TIME:
            // State Section
            activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                     "APP:ACTIVITY_MESSAGES:GAMESTATES:STRATEGY_TIME",
                                                     LanguageManager::getSystemLanguage())
                                                     .c_str()));
            break;
        case GameState::PRE_GAME:
        case GameState::GAME:
        {
            // State Section
            // If watching, show the networth
            if (playerStatus == PlayerStatus::WATCHING)
            {
                std::string netWorth = getNetWorth(data);
                activity.SetState(const_cast<char *>(netWorth.c_str()));
            }
            else
            {
                if (gameState == GameState::PRE_GAME)
                    activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                             "APP:ACTIVITY_MESSAGES:GAMESTATES:PRE_GAME",
                                                             LanguageManager::getSystemLanguage())
                                                             .c_str()));
                else
                    activity.SetState(const_cast<char *>(LanguageManager::getString(
                                                             "APP:ACTIVITY_MESSAGES:GAMESTATES:GAME",
                                                             LanguageManager::getSystemLanguage())
                                                             .c_str()));
            }

            // Time Section
            now += std::chrono::seconds(-gameTime);
            int64_t time0InMatch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            fixGameTimeIfNecessary(time0InMatch);
            if (gameState == GameState::PRE_GAME)
                activity.GetTimestamps().SetEnd(DiscordTimestamp(time0InMatch));
            else
                activity.GetTimestamps().SetStart(DiscordTimestamp(time0InMatch));

            // Small Image Section
            long long matchId = getMatchId(data);
            std::string matchIdText = LanguageManager::getString(
                "APP:ACTIVITY_MESSAGES:MATCH:INFO_ID",
                LanguageManager::getSystemLanguage());

            StringExtensions::findAndReplaceAll(matchIdText, "{{ID}}", std::to_string(matchId));
            std::string neutralImageKey = getNeutralNameBasedOnMatchId(matchId);

            activity.GetAssets().SetSmallImage(neutralImageKey.c_str());
            activity.GetAssets().SetSmallText(matchIdText.c_str());

            break;
        }
        case GameState::NONE:
        default:
        {
            return;
        }
        }

        break;
    }
    }

    discordService->updateActivity(activity);
}

// };

// class DotaService
// {
//     static DotaService *instance;

//     int64_t currentMatchTime = 0;
//     std::map<std::string, std::string> WorkshopMapsCache;

//

//

//

// public:

// DotaService *DotaService::instance = 0;
