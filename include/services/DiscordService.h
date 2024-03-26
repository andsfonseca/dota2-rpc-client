#pragma once

#include <thread>
#include <string>

#include <discord/discord.h>

class DiscordService
{
private:
    DiscordService();
    bool initialize();
    void loop();
private:
    static DiscordService *instance;
    std::thread *threadHandler;
    std::shared_ptr <discord::Core> core;
    volatile bool started{false};
    volatile bool interrupted{false};
    volatile std::time_t lastUpdate;
public:
    static DiscordService *getInstance();
    void updateActivity(discord::Activity activity);
    void cleanActivity();
    bool start();
    void stop();
    std::string getLanguage();

};