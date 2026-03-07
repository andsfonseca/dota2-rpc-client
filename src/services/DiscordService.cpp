#include <services/DiscordService.h>
#include <managers/ConfigurationManager.h>

#include <iostream>

DiscordService *DiscordService::instance = nullptr;
std::mutex DiscordService::instanceMutex;

DiscordService::DiscordService() {}

bool DiscordService::initialize()
{
    
    // Check if already initialized
    if (core) {
        return true;
    }
    
    // Instatiante Discord
    discord::Core *aux{};
    auto result = discord::Core::Create(963884877428711434, DiscordCreateFlags_NoRequireDiscord, &aux);

    // Update reference
    this->core.reset(aux);
    if (!this->core)
    {
        std::cout << "Failed to instantiate discord core! (err "
                  << static_cast<int>(result)
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
    // Loop
    while (!interrupted.load())
    {
        int64_t t = static_cast<int64_t>(std::time(0));
        double seconds = static_cast<double>(t - lastUpdate.load());

        // With no updates in 10 seconds, stop the loop
        if (seconds > 10)
        {
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(coreMutex);
            if (!this->core)
            {
                break;
            }
            
            this->core->RunCallbacks();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    // Clean up when loop ends
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        this->core.reset();
    }
    
    // Mark as no longer running
    started.store(false);
}

DiscordService *DiscordService::getInstance()
{
    if (!instance)
    {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance)
        {
            instance = new DiscordService;
        }
    }
    return instance;
}

void DiscordService::updateActivity(discord::Activity activity)
{
    if (!start())
        return;

    lastUpdate.store(static_cast<int64_t>(std::time(0)));
    
    std::lock_guard<std::mutex> lock(coreMutex);
    if (this->core)
    {
        this->core->ActivityManager().UpdateActivity(activity, [](discord::Result result)
                                                     {
                                                        if(result != discord::Result::Ok)
                                                        {
                                                            std::cout << "Failed updating activity! ("
                                                                        << static_cast<int>(result)
                                                                        << ")" << std::endl;
                                                        } });
    }
}

void DiscordService::cleanActivity()
{
    bool wasStarted = started.load();
    
    // Clear the activity if it's running
    if (wasStarted)
    {
        {
            std::lock_guard<std::mutex> lock(coreMutex);
            if (this->core)
            {
                this->core->ActivityManager().ClearActivity([](discord::Result result)
                                                            {   
                                                                if(result != discord::Result::Ok)
                                                                {
                                                                    std::cout << "Failed clearing activity! ("
                                                                                << static_cast<int>(result)
                                                                                << ")" << std::endl;
                                                                }
                                                            });
            }
        }
        
        // Stop the Discord service
        stop();
    }
    else
    {
        // Even if not started, clean up any leftover thread
        if (threadHandler && threadHandler->joinable())
        {
            threadHandler->join();
            threadHandler.reset();
        }
    }
}

bool DiscordService::start()
{
    // Check if we need to clean up a finished thread
    if (!started.load() && threadHandler && threadHandler->joinable())
    {
        threadHandler->join();
        threadHandler.reset();
    }
    
    // Initialize core if needed
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        if (!this->core)
        {
            if (!initialize())
                return false;
        }
    }
    
    // Start thread if not already started
    bool expected = false;
    if (started.compare_exchange_strong(expected, true))
    {
        // Reset interrupted flag when starting new thread
        interrupted.store(false);
        lastUpdate.store(static_cast<int64_t>(std::time(0)));
        threadHandler = std::make_unique<std::thread>(&DiscordService::loop, this);
    }
    
    return true;
}

void DiscordService::stop()
{
    // Signal stop to the thread
    interrupted.store(true);
    
    // Wait for thread to finish if it's running
    if (threadHandler && threadHandler->joinable())
    {
        threadHandler->join();
    }
    
    // Clean up resources 
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        if (this->core)
        {
            this->core.reset();
        }
    }
    
    // Reset state
    threadHandler.reset();
    interrupted.store(false);
    started.store(false);
}

std::string DiscordService::getLanguage()
{
    // Initialize
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        if (!this->core)
        {
            if (!initialize())
                return "";
        }

        char locale[128];
        this->core->ApplicationManager().GetCurrentLocale(locale);
        return std::string(locale);
    }
}