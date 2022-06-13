#pragma once

#include <map>
#include <string>
#include <json/json.h>

enum class PlayerStatus
{
    STAND_BY,
    PLAYING,
    WATCHING,
    COACHING
};

enum class GameState
{
    NONE,
    HERO_SELECTION,
    STRATEGY_TIME,
    PRE_GAME,
    GAME
};

enum class ItemStatusEffect
{
    WITHOUT_ITEMS,
    SHARD,
    SCEPTER,
    SCEPTER_AND_SHARD,
    SMOKE,
    AEGIS
};

class DotaService
{
private:
    DotaService();
    void fixGameTimeIfNecessary(int64_t &matchTime);
    GameState getCurrentGameState(Json::Value data);
    int getGameTimeElapsed(Json::Value data);
    int getHeroLevel(Json::Value data);
    std::string getHeroName(Json::Value data);
    ItemStatusEffect getItemStatusEffect(Json::Value data);
    void getKillDeathAssists(Json::Value data, int &kill, int &death, int &assist);
    std::string getMapName(Json::Value data);
    long long getMatchId(Json::Value data);
    int getMatchTimeElapsed(Json::Value data);
    std::string getNetWorth(Json::Value data);
    std::string getNeutralNameBasedOnMatchId(long long i);
    int getPlayerGold(Json::Value data);
    void getPlayerHits(Json::Value data, int &lastHits, int &denies);
    PlayerStatus getPlayerStatus(Json::Value data);
    std::string getWorkshopMapName(const std::string path);
    bool playerHasAegis(Json::Value data);
    std::string resolveHeroName(std::string key);
private:
    static DotaService *instance;
    int64_t currentMatchTime = 0;
    std::string currentLocale;
    std::map<std::string, std::string> WorkshopMapsCache;
public:
    static DotaService *getInstance();
    void interpretJson(Json::Value data);
};
