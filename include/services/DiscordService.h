#pragma once

#include <thread>
#include <string>
#include <mutex>
#include <atomic>
#include <memory>
#include <cstdint>
#include <ctime>

#include "../../third_party/discord-sdk-src/cpp/discord.h"

class DiscordService
{
private:
    DiscordService();
    ~DiscordService();
#ifdef __linux__
    std::string getXDGRuntimeDir();
    std::vector<std::string> getIPCPaths(const std::string &runtimeDir, bool recursive = false);
    bool setupDiscordIPC();
#endif
    bool initialize();
    void loop();

private:
    static DiscordService *instance;
    static std::mutex instanceMutex;

    std::unique_ptr<std::thread> threadHandler;
    std::shared_ptr<discord::Core> core;
    std::mutex coreMutex;

    std::atomic<bool> started{false};
    std::atomic<bool> interrupted{false};
    std::atomic<int64_t> lastUpdate{0};

#ifdef __linux__
    bool ipcSymlinkCreated{false};
    std::string ipcSymlinkPath;
#endif

public:
    static DiscordService *getInstance();
    void updateActivity(discord::Activity activity);
    void cleanActivity();
    bool start();
    void stop();
    std::string getLanguage();
#ifdef __linux__
    void cleanupDiscordIPC();
#endif
};