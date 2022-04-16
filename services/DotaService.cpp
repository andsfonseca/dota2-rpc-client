#include <iostream>
#include <json/json.h>

enum PlayerStatus
{
    STANDBY,
    PLAYING,
    WATCHING,
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
            return PlayerStatus::STANDBY;

        if (!data["player"]["team2"].isNull() && !data["player"]["team3"].isNull())
            return PlayerStatus::WATCHING;

        if (data["player"]["activity"].isNull())
            return PlayerStatus::STANDBY;

        std::string activity = data["player"]["activity"].asString();

        if (activity != "playing")
            std::cout << "Unknow activity '" << activity << "\n";

        return PlayerStatus::PLAYING;
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
            std::cout << "Playing" << "\n";
            break;
        case PlayerStatus::WATCHING:
            std::cout << "Watching" << "\n";
            break;
        case PlayerStatus::STANDBY:
        default:
            std::cout << "StandBy" << "\n";
            break;
        }

        // Update If Not Dota 2 updates
    }
};

DotaService *DotaService::instance = 0;
