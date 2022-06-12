#include <services/DiscordService.h>
#include <managers/ConfigurationManager.h>

#include <iostream>

DiscordService *DiscordService::instance = nullptr;

DiscordService::DiscordService() {}

bool DiscordService::initialize()
{
    // Instatiante Discord
    discord::Core *aux{};
    auto result = discord::Core::Create(963884877428711434, DiscordCreateFlags_NoRequireDiscord, &aux);

    // Update reference
    this->core.reset(aux);
    if (!this->core)
    {
        std::cout << "Failed to instantiate discord core! (err " << static_cast<int>(result)
                  << ")" << std::endl;
        return false;
    }

    // Reload configurations after init
    ConfigurationManager::load();

    // Error Log
    this->core->SetLogHook(
        discord::LogLevel::Debug, [](discord::LogLevel level, const char *message)
        { std::cerr << "Log(" << static_cast<uint32_t>(level) << "): " << message << std::endl; });

    return true;
}

void DiscordService::loop()
{
    bool breaked{false};

    // Loop
    do
    {
        std::time_t t = std::time(0);
        double seconds = std::difftime(t, lastUpdate);

        // With no updates in 10 seconds
        if (seconds > 10)
        {
            breaked = true;
            break;
        }

        this->core->RunCallbacks();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

    } while (!interrupted);

    // If Stopped reset the values
    if (breaked && !interrupted)
    {
        interrupted = false;
        started = false;
        this->core.reset();
    }
}

DiscordService *DiscordService::getInstance()
{
    if (!instance)
        instance = new DiscordService;
    return instance;
}

void DiscordService::updateActivity(discord::Activity activity)
{
    if (!start())
        return;

    lastUpdate = std::time(0);

    this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
                                                 {
                                                         if(result != discord::Result::Ok)
                                                         std::cout << "Failed updating activity!" << std::endl; });
}

void DiscordService::cleanActivity()
{
    if (!started)
        return;

    // Remove the activity
    this->core->ActivityManager().ClearActivity([](discord::Result result)
                                                { if(result != discord::Result::Ok)
                                                         std::cout << "Failed updating activity!\n"; });
    // Stop the instance
    stop();
}

bool DiscordService::start()
{
    // Initialize
    if (!this->core)
    {
        if (!initialize())
            return false;
    }
    // Start if not started
    if (!started)
    {
        started = true;
        threadHandler = new std::thread(&DiscordService::loop, this);
    }
    return true;
}

void DiscordService::stop()
{
    // If start, can stop
    if (started)
    {
        // Send interrupted signal
        interrupted = true;
        threadHandler->join();

        // Reset the values
        interrupted = false;
        started = false;
        this->core.reset();
    }
}

std::string DiscordService::getLanguage(){
    // Initialize
    if (!this->core)
    {
        if (!initialize())
            return "";
    }

    char locale[128];

    this->core->ApplicationManager().GetCurrentLocale(locale);

    return std::string(locale);
}