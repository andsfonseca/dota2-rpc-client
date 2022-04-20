#include <iostream>
#include <chrono>
#include <json/json.h>

#include "DiscordService.cpp"
#include "../utils/NamesDota2.hpp"

enum PlayerStatus
{
    STAND_BY,
    PLAYING,
    WATCHING,
};

enum GameState
{
    NONE,
    HERO_SELECTION,
    STRATEGY_TIME,
    PRE_GAME,
    GAME
};

enum ItemStatusEffect
{
    WITHOUT_ITEMS,
    SHARD,
    SCEPTER,
    SCEPTER_AND_SHARD,
    SMOKE
};

class DotaService
{
    static DotaService *instance;

    // Private constructor so that no objects can be created.
    DotaService()
    {
    }

    PlayerStatus IsThePlayerInMatchUp(Json::Value data)
    {
        if (data["player"].isNull())
            return PlayerStatus::STAND_BY;

        if (!data["player"]["team2"].isNull() && !data["player"]["team3"].isNull())
            return PlayerStatus::WATCHING;

        if (data["player"]["activity"].isNull())
            return PlayerStatus::STAND_BY;

        std::string activity = data["player"]["activity"].asString();

        if (activity != "playing")
            std::cout << "Unknow activity '" << activity << "\n";

        return PlayerStatus::PLAYING;
    }

    GameState GetCurrentGameState(Json::Value data)
    {
        if (data["map"].isNull())
        {
            return GameState::NONE;
        }

        if (data["map"]["game_state"].isNull())
        {
            return GameState::NONE;
        }

        std::string gamestate = data["map"]["game_state"].asString();

        if (gamestate == "DOTA_GAMERULES_STATE_INIT" || gamestate == "DOTA_GAMERULES_STATE_WAIT_FOR_PLAYERS_TO_LOAD" || gamestate == "DOTA_GAMERULES_STATE_HERO_SELECTION")
        {
            return GameState::HERO_SELECTION;
        }
        else if (gamestate == "DOTA_GAMERULES_STATE_STRATEGY_TIME" || gamestate == "DOTA_GAMERULES_STATE_WAIT_FOR_MAP_TO_LOAD" || gamestate == "DOTA_GAMERULES_STATE_TEAM_SHOWCASE")
        {
            return GameState::STRATEGY_TIME;
        }
        else if (gamestate == "DOTA_GAMERULES_STATE_PRE_GAME")
        {
            return GameState::PRE_GAME;
        }
        else if (gamestate == "DOTA_GAMERULES_STATE_GAME_IN_PROGRESS")
        {
            return GameState::GAME;
        }
        else if (gamestate == "DOTA_GAMERULES_STATE_POST_GAME")
        {
            return GameState::NONE;
        }
        else
        {
            std::cout << "Unknown Gamestate: " << gamestate << "\n";
            return GameState::NONE;
        }
    }

    int GetGameTimeElapsed(Json::Value data)
    {
        if (data["map"].isNull())
        {
            return 0;
        }

        if (data["map"]["clock_time"].isNull())
        {
            return 0;
        }

        int time = data["map"]["clock_time"].asInt();

        return time;
    }

    int GetMatchTimeElapsed(Json::Value data)
    {
        if (data["map"].isNull())
        {
            return 0;
        }

        if (data["map"]["game_time"].isNull())
        {
            return 0;
        }

        int time = data["map"]["game_time"].asInt();

        return time;
    }

    std::string GetHeroName(Json::Value data)
    {
        if (data["hero"].isNull())
        {
            return "";
        }

        if (data["hero"]["name"].isNull())
        {
            return "";
        }

        std::string name = data["hero"]["name"].asString();

        // Weareables

        if (data["wearables"].isNull())
            return name;

        for (int i = 0;; i++)
        {
            std::string key = "wearable" + std::to_string(i);

            if (data["wearables"][key].isNull())
            {
                break;
            }

            int itemId = data["wearables"][key].asInt();

            // Personas First
            if ((name == "npc_dota_hero_antimage" && itemId == 13783) ||
                (name == "npc_dota_hero_dragon_knight" && itemId == 18113) ||
                (name == "npc_dota_hero_mirana" && itemId == 18178) ||
                (name == "npc_dota_hero_invoker" && itemId == 13042))
            {
                return name + "_2";
            }
            if (name == "npc_dota_hero_pudge" && itemId == 13786)
            {
                return name + "_4";
            }

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

    std::string ResolveHeroName(std::string name)
    {
        if (HERO_NAMES.count(name))
        {
            return HERO_NAMES.find(name)->second;
        }
        return name;
    }

    int GetHeroLevel(Json::Value data)
    {
        if (data["hero"].isNull())
        {
            return 1;
        }

        if (data["hero"]["level"].isNull())
        {
            return 1;
        }

        int level = data["hero"]["level"].asInt();

        return level;
    }

    void GetKillDeathAssists(Json::Value data, int &kill, int &death, int &assist)
    {
        kill = 0;
        death = 0;
        assist = 0;

        if (data["player"].isNull())
        {
            return;
        }

        if (data["player"]["kills"].isNull() || data["player"]["assists"].isNull() || data["player"]["deaths"].isNull())
        {
            return;
        }

        kill = data["player"]["kills"].asInt();
        death = data["player"]["deaths"].asInt();
        assist = data["player"]["assists"].asInt();
    }

    long long GetMatchId(Json::Value data)
    {
        if (data["map"].isNull())
        {
            return 0;
        }

        if (data["map"]["matchid"].isNull())
        {
            return 0;
        }

        std::string matchId = data["map"]["matchid"].asString();

        std::stringstream in;
        in << matchId;
        long long i;
        in >> i;

        return i;
    }

    std::string GetNeutralNameBasedOnMatchId(long long i)
    {
        return NEUTRAL_NAMES[i % 35];
    }

    std::string GetNetWorth(Json::Value data)
    {

        int radiant = 0;
        int dire = 0;

        if (data["player"].isNull())
        {
            return "⛰️Radiant | Dire🌋";
        }

        if (data["player"]["team2"].isNull() || data["player"]["team3"].isNull())
        {
            return "⛰️Radiant | Dire🌋";
        }

        int i = 0;

        for (;; i++)
        {
            std::string player = "player" + std::to_string(i);
            if (data["player"]["team2"][player].isNull())
            {
                break;
            }

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
            {
                break;
            }

            if (!data["player"]["team3"][player]["net_worth"].isNull())
            {
                int count = data["player"]["team3"][player]["net_worth"].asInt();
                dire += count;
            }
        }

        std::string status;
        if (radiant > dire)
        {
            int value = (radiant - dire) / 1000;
            status = "⛰️Radiant " + std::to_string(value) + "k | Dire🌋";
        }
        else if (dire > radiant)
        {
            int value = (dire - radiant) / 1000;
            status = "⛰️Radiant | " + std::to_string(value) + "k Dire🌋";
        }
        else
        {
            status = "⛰️Radiant | Dire🌋";
        }

        return status;
    }

    ItemStatusEffect GetItemStatusEffect(Json::Value data)
    {
        if (data["hero"].isNull())
            return ItemStatusEffect::WITHOUT_ITEMS;

        if (data["hero"]["smoked"].isNull() || data["hero"]["aghanims_shard"].isNull() || data["hero"]["aghanims_scepter"].isNull())
            return ItemStatusEffect::WITHOUT_ITEMS;

        bool smoked = data["hero"]["smoked"].asBool();

        if (smoked)
            return ItemStatusEffect::SMOKE;

        bool aghanimsShard = data["hero"]["aghanims_shard"].asBool();
        bool aghanimsScepter = data["hero"]["aghanims_scepter"].asBool();

        if (aghanimsShard && aghanimsScepter)
            return ItemStatusEffect::SCEPTER_AND_SHARD;
        else if (aghanimsScepter)
            return ItemStatusEffect::SCEPTER;
        else if (aghanimsShard)
            return ItemStatusEffect::SHARD;
        return ItemStatusEffect::WITHOUT_ITEMS;
    }

    void GetPlayerHits(Json::Value data, int &lastHits, int &denies){
        lastHits = 0;
        denies = 0;

        if (data["player"].isNull())
        {
            return;
        }

        if (data["player"]["last_hits"].isNull() || data["player"]["denies"].isNull())
        {
            return;
        }

        lastHits = data["player"]["last_hits"].asInt();
        denies = data["player"]["denies"].asInt();
    }

    int GetPlayerGold(Json::Value data){
        if (data["player"].isNull())
        {
            return 0;
        }

        if (data["player"]["gold"].isNull())
        {
            return 0;
        }

        return data["player"]["gold"].asInt();
    }
public:
    static DotaService *getInstance()
    {
        if (!instance)
            instance = new DotaService;
        return instance;
    }

    void InterpretJsonFile(trantor::Date requestDate, Json::Value data)
    {

        DiscordService *discordService = discordService->getInstance();
        discord::Activity activity{};
        auto now = std::chrono::system_clock::now();

        int gameTime;
        int matchTime;
        int64_t timeToStart;
        int64_t timeAfterStart;
        std::string heroName;
        std::string npcName;
        int level;
        int kill = -1;
        int death = -1;
        int assist = -1;
        std::string kda;
        std::string matchIdText;
        std::string net;

        // Identify In-Game and Outside-Game
        switch (IsThePlayerInMatchUp(data))
        {
        case PlayerStatus::PLAYING:
        {
            activity.SetType(discord::ActivityType::Playing);

            GameState state1 = GetCurrentGameState(data);

            gameTime = GetGameTimeElapsed(data);
            matchTime = GetMatchTimeElapsed(data);

            switch (state1)
            {
            case GameState::HERO_SELECTION:
                npcName = NEUTRAL_NAMES[2];

                activity.SetDetails(const_cast<char *>("Choosing a hero"));
                activity.SetState(const_cast<char *>("Hero Selection"));
                activity.GetAssets().SetLargeImage(npcName.c_str());
                break;
            case GameState::STRATEGY_TIME:
                npcName = GetHeroName(data);
                heroName = "Playing as " + ResolveHeroName(npcName);

                activity.GetAssets().SetLargeImage(npcName.c_str());
                activity.SetDetails(const_cast<char *>(heroName.c_str()));
                activity.SetState(const_cast<char *>("Strategy Time"));
                break;
            case GameState::PRE_GAME:
            {

                level = GetHeroLevel(data);
                GetKillDeathAssists(data, kill, death, assist);
                npcName = GetHeroName(data);

                heroName = "Playing as " + ResolveHeroName(npcName) + " - Lvl." + std::to_string(level);
                kda = std::to_string(kill) + " / " + std::to_string(death) + " / " + std::to_string(assist);

                now += std::chrono::seconds(-gameTime);
                timeToStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
                int lastHits;
                int denies;

                GetPlayerHits(data, lastHits, denies);
                
                std::string imageText = "💰 "+ std::to_string(GetPlayerGold(data)) + " | LH: " + std::to_string(lastHits) + " | DN: " + std::to_string(denies);

                ItemStatusEffect effect = GetItemStatusEffect(data);

                switch (effect)
                {
                case ItemStatusEffect::SMOKE:
                    activity.GetAssets().SetSmallImage("smoke_of_deceit");
                    activity.GetAssets().SetSmallText("Smoked");
                    break;
                case ItemStatusEffect::SCEPTER_AND_SHARD:
                    activity.GetAssets().SetSmallImage("aghanims_scepter_2");
                    activity.GetAssets().SetSmallText("has Aghanim's Scepter and Shard");
                    break;
                case ItemStatusEffect::SCEPTER:
                    activity.GetAssets().SetSmallImage("aghanims_scepter");
                    activity.GetAssets().SetSmallText("has Aghanim's Scepter");
                    break;
                 case ItemStatusEffect::SHARD:
                    activity.GetAssets().SetSmallImage("aghanims_shard");
                    activity.GetAssets().SetSmallText("has Aghanim's Shard");
                    break;
                case ItemStatusEffect::WITHOUT_ITEMS:
                default:
                    break;
                }
                
                activity.GetAssets().SetLargeImage(npcName.c_str());
                activity.GetAssets().SetLargeText(imageText.c_str());
                activity.GetTimestamps().SetEnd(DiscordTimestamp(timeToStart));
                activity.SetDetails(const_cast<char *>(heroName.c_str()));
                activity.SetState(const_cast<char *>(kda.c_str()));
                break;
            }
            case GameState::GAME:
            {

                level = GetHeroLevel(data);
                GetKillDeathAssists(data, kill, death, assist);
                npcName = GetHeroName(data);
                heroName = "Playing as " + ResolveHeroName(npcName) + " - Lvl." + std::to_string(level);
                kda = std::to_string(kill) + " / " + std::to_string(death) + " / " + std::to_string(assist);

                now += std::chrono::seconds(-gameTime);
                timeAfterStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                int lastHits;
                int denies;

                GetPlayerHits(data, lastHits, denies);
                std::string imageText = "💰 "+ std::to_string(GetPlayerGold(data)) + " | LH: " + std::to_string(lastHits) + " | DN: " + std::to_string(denies);

                ItemStatusEffect effect = GetItemStatusEffect(data);
                switch (effect)
                {
                case ItemStatusEffect::SMOKE:
                    activity.GetAssets().SetSmallImage("smoke_of_deceit");
                    activity.GetAssets().SetSmallText("Smoked");
                    break;
                case ItemStatusEffect::SCEPTER_AND_SHARD:
                    activity.GetAssets().SetSmallImage("aghanims_scepter_2");
                    activity.GetAssets().SetSmallText("has Aghanim's Scepter and Shard");
                    break;
                case ItemStatusEffect::SCEPTER:
                    activity.GetAssets().SetSmallImage("aghanims_scepter");
                    activity.GetAssets().SetSmallText("has Aghanim's Scepter");
                    break;
                 case ItemStatusEffect::SHARD:
                    activity.GetAssets().SetSmallImage("aghanims_shard");
                    activity.GetAssets().SetSmallText("has Aghanim's Shard");
                    break;
                case ItemStatusEffect::WITHOUT_ITEMS:
                default:
                    break;
                }
                
                activity.GetAssets().SetLargeImage(npcName.c_str());
                activity.GetAssets().SetLargeText(imageText.c_str());
                activity.GetTimestamps().SetStart(DiscordTimestamp(timeAfterStart));
                activity.SetDetails(const_cast<char *>(heroName.c_str()));
                activity.SetState(const_cast<char *>(kda.c_str()));
                break;
            }
            case GameState::NONE:
            default:
                return;
            }

            discordService->UpdateActivity(activity);
            break;
        }
        case PlayerStatus::WATCHING:
        {
            long long matchId = GetMatchId(data);
            npcName = GetNeutralNameBasedOnMatchId(matchId);
            activity.GetAssets().SetLargeImage(const_cast<char *>("watching_default"));

            activity.SetDetails(const_cast<char *>("Watching a match"));
            activity.SetType(discord::ActivityType::Watching);

            GameState state = GetCurrentGameState(data);

            gameTime = GetGameTimeElapsed(data);
            matchTime = GetMatchTimeElapsed(data);

            switch (state)
            {
            case GameState::HERO_SELECTION:
                activity.SetState(const_cast<char *>("Hero Selection"));
                break;
            case GameState::STRATEGY_TIME:
                activity.SetState(const_cast<char *>("Strategy Time"));
                break;
            case GameState::PRE_GAME:
                now += std::chrono::seconds(-gameTime);
                timeToStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                matchIdText = "Match Id: " + std::to_string(matchId);
                net = GetNetWorth(data);
                activity.GetAssets().SetSmallImage(npcName.c_str());
                activity.GetAssets().SetSmallText(matchIdText.c_str());
                activity.GetTimestamps().SetEnd(DiscordTimestamp(timeToStart));
                activity.SetState(const_cast<char *>(net.c_str()));
                break;
            case GameState::GAME:

                now += std::chrono::seconds(-gameTime);
                timeAfterStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                matchIdText = "Match Id: " + std::to_string(matchId);

                net = GetNetWorth(data);

                activity.GetAssets().SetSmallImage(npcName.c_str());
                activity.GetAssets().SetSmallText(matchIdText.c_str());
                activity.GetTimestamps().SetStart(DiscordTimestamp(timeAfterStart));
                activity.SetState(const_cast<char *>(net.c_str()));
                break;
            case GameState::NONE:
            default:
                return;
                break;
            }

            discordService->UpdateActivity(activity);
            break;
        }
        case PlayerStatus::STAND_BY:
        default:
            discordService->CleanActivity();
        }
    }
};

DotaService *DotaService::instance = 0;
