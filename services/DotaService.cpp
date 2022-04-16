#include <iostream>
#include <json/json.h>

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

    GameState FindCurrentGameState(Json::Value data)
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
        else
        {
            std::cout << "Unknown Gamestate: " << gamestate << "\n";
            return GameState::NONE;
        }
    }


public:
    static DotaService *getInstance()
    {
        if (!instance)
            instance = new DotaService;
        return instance;
    }

    void InterpretJsonFile(trantor::Date date, Json::Value data)
    {
        std::cout << "==================================="
                  << "\n";
        // std::cout << data << "\n";

        // Identify In-Game and Outside-Game
        switch (IsThePlayerInMatchUp(data))
        {
        case PlayerStatus::PLAYING:
            break;
        case PlayerStatus::WATCHING:
        {
            GameState state = FindCurrentGameState(data);
            switch (state)
            {
            case GameState::HERO_SELECTION:
                std::cout << "Selecao de Heroi"
                          << "\n";
                break;
            case GameState::STRATEGY_TIME:
                std::cout << "Tempo de estrategia"
                          << "\n";
                break;
            case GameState::PRE_GAME:
                std::cout << "Pre-game"
                          << "\n";
                break;
            case GameState::GAME:
                std::cout << "Partida"
                          << "\n";
                break;
            case GameState::NONE:
            default:
                break;
            }
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
