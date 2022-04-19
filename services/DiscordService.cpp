#include <iostream>
#include <thread>
#include "../third_party/discord-sdk-src/cpp/discord.h"

class DiscordService
{
    static DiscordService *instance;
    std::thread *threadHandler;

    std::unique_ptr<discord::Core> core;
    bool started{false};
    volatile bool interrupted{false};

    // Private constructor so that no objects can be created.
    DiscordService()
    {
    }

    bool Initialize()
    {
        discord::Core *aux{};
        auto result = discord::Core::Create(963884877428711434, DiscordCreateFlags_NoRequireDiscord, &aux);
        this->core.reset(aux);
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

    void Loop()
    {
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

    void UpdateActivity(discord::Activity activity)
    {
        if (!Start())
            return;
        this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
                                                     { 
                                                         if(result != discord::Result::Ok)
                                                         std::cout << "Failed updating activity!\n"; });
    }

    void CleanActivity()
    {
        if (!Start())
            return;
        this->core->ActivityManager().ClearActivity([](discord::Result result)
                                                    { if(result != discord::Result::Ok)
                                                         std::cout << "Failed updating activity!\n"; });
        Stop();
    }

    bool Start()
    {
        if (!this->core)
        {
            if (!Initialize())
                return false;
        }
        if (!started)
        {
            started = true;
            threadHandler = new std::thread(&DiscordService::Loop, this);
        }
        return true;
    }

    void Stop()
    {
        if (started)
        {
            interrupted = true;
            threadHandler->join();
            interrupted = false;
            started = false;
            this->core.reset();
        }
    }
};

DiscordService *DiscordService::instance = 0;
