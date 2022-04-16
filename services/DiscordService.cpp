#pragma once

#include <iostream>
#include <thread>
#include "../third_party/discord-sdk-src/cpp/discord.h"

class DiscordService
{
    static DiscordService *instance;
    discord::Core *core{};
    volatile bool interrupted{false};
    std::thread *threadHandler;

    // Private constructor so that no objects can be created.
    DiscordService()
    {
    }

    void Loop()
    {
        if (!instance)
            return;
        do

        {
            this->core->RunCallbacks();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        } while (!interrupted);
    }

public:
    static DiscordService *getInstance()
    {
        if (!instance)
            instance = new DiscordService;
        return instance;
    }

    bool Initialize()
    {
        auto result = discord::Core::Create(963884877428711434, DiscordCreateFlags_Default, &core);

        if (!this->core)
        {
            std::cout << "Failed to instantiate discord core! (err " << static_cast<int>(result)
                      << ")\n";
            return false;
        }

        this->core->SetLogHook(
            discord::LogLevel::Debug, [](discord::LogLevel level, const char *message)
            { std::cerr << "Log(" << static_cast<uint32_t>(level) << "): " << message << "\n"; });

        return true;
    }

    void UpdateActivity(std::string details, std::string state)
    {
        discord::Activity activity{};
        activity.SetDetails(const_cast<char *>(details.c_str()));
        activity.SetState(const_cast<char *>(state.c_str()));
        activity.GetAssets().SetSmallText("i mage");
        activity.GetAssets().SetLargeText("u mage");
        activity.SetType(discord::ActivityType::Playing);
        this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
                                                     { 
                                                         if(result != discord::Result::Ok)
                                                         std::cout << "Failed updating activity!\n"; });
    }

    void Start()
    {
        threadHandler = new std::thread(&DiscordService::Loop, this);
    }

    void Stop()
    {
        interrupted = true;
        threadHandler->join();
    }
};

DiscordService *DiscordService::instance = 0;
