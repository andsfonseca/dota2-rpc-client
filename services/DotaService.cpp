#include <iostream>
#include <chrono>
#include <json/json.h>

#include "DiscordService.cpp"

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

    void FindScoreboard(Json::Value data, int &radiant, int &dire)
    {
        radiant = 0;
        dire = 0;

        if (data["player"].isNull())
        {
            return;
        }

        if (data["player"]["team2"].isNull() || data["player"]["team2"].isNull())
        {
            return;
        }

        // Radiant
        int i = 0;
        for (i = 0; i < 5; i++)
        {
            std::string player = "player" + std::to_string(i);
            if (data["player"]["team2"][player]["kills"].isNull())
                continue;
            radiant = radiant + data["player"]["team2"][player]["kills"].asInt();
        }
        // Dire
        for (; i < 10; i++)
        {
            std::string player = "player" + std::to_string(i);
            if (data["player"]["team3"][player]["kills"].isNull())
                continue;
            dire = dire + data["player"]["team3"][player]["kills"].asInt();
        }
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

    void getKillDeathAssists(Json::Value data, int &kill, int &death, int &assist)
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
        int radiant;
        int dire;
        std::string gamescoreboard;
        std::string heroname;
        int level;
        int kill = -1;
        int death = -1;
        int assist = -1;
        std::string kda;

        std::cout << "==================================="
                  << "\n";
        std::cout << data << "\n";

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
                activity.SetDetails(const_cast<char *>("Choosing a hero"));
                activity.SetState(const_cast<char *>("Hero Selection"));
                break;
            case GameState::STRATEGY_TIME:

                heroname = "Playing as " + GetHeroName(data);

                activity.SetDetails(const_cast<char *>(heroname.c_str()));
                activity.SetState(const_cast<char *>("Strategy Time"));
                break;
            case GameState::PRE_GAME:

                level = GetHeroLevel(data);
                getKillDeathAssists(data, kill, death, assist);
                heroname = "Playing as " + GetHeroName(data) + " - Lvl." + std::to_string(level);
                kda = std::to_string(kill) + " / " + std::to_string(assist) + " / " + std::to_string(death);

                now += std::chrono::seconds(-gameTime);
                timeToStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                activity.GetTimestamps().SetEnd(DiscordTimestamp(timeToStart));
                activity.SetDetails(const_cast<char *>(heroname.c_str()));
                activity.SetState(const_cast<char *>(kda.c_str()));
                break;
            case GameState::GAME:

                level = GetHeroLevel(data);
                getKillDeathAssists(data, kill, death, assist);
                heroname = "Playing as " + GetHeroName(data) + " - Lvl." + std::to_string(level);
                kda = std::to_string(kill) + " / " + std::to_string(assist) + " / " + std::to_string(death);

                now += std::chrono::seconds(-gameTime);
                timeAfterStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                FindScoreboard(data, radiant, dire);
                gamescoreboard = "Scoreboard: " + std::to_string(radiant) + " - " + std::to_string(dire);

                activity.GetTimestamps().SetStart(DiscordTimestamp(timeAfterStart));
                activity.SetDetails(const_cast<char *>(heroname.c_str()));
                activity.SetState(const_cast<char *>(kda.c_str()));
                break;
            case GameState::NONE:
            default:
                std::cout << "Game Time: " << gameTime << "\n";
                std::cout << "Match Time: " << matchTime << "\n";
                break;
            }

            discordService->UpdateActivity(activity);
            break;
        }
        case PlayerStatus::WATCHING:
        {
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

                FindScoreboard(data, radiant, dire);
                gamescoreboard = "Scoreboard: " + std::to_string(radiant) + " - " + std::to_string(dire);

                activity.GetTimestamps().SetEnd(DiscordTimestamp(timeToStart));
                activity.SetState(const_cast<char *>(gamescoreboard.c_str()));
                break;
            case GameState::GAME:

                now += std::chrono::seconds(-gameTime);
                timeAfterStart = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

                FindScoreboard(data, radiant, dire);
                gamescoreboard = "Scoreboard: " + std::to_string(radiant) + " - " + std::to_string(dire);

                activity.GetTimestamps().SetStart(DiscordTimestamp(timeAfterStart));
                activity.SetState(const_cast<char *>(gamescoreboard.c_str()));
                break;
            case GameState::NONE:
            default:
                std::cout << "Game Time: " << gameTime << "\n";
                std::cout << "Match Time: " << matchTime << "\n";
                break;
            }

            discordService->UpdateActivity(activity);
            break;
        }
        case PlayerStatus::STAND_BY:
        default:
            break;
        }

        // Update If Not Dota 2 updates
    }
};

DotaService *DotaService::instance = 0;
