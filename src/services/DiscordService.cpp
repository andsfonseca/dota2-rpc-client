#include <services/DiscordService.h>
#include <managers/ConfigurationManager.h>

#include <iostream>
#ifdef __linux__
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>
#endif
#include <cstdlib>
#include <sstream>

DiscordService *DiscordService::instance = nullptr;
std::mutex DiscordService::instanceMutex;

DiscordService::DiscordService() {}

DiscordService::~DiscordService()
{
#ifdef __linux__
    cleanupDiscordIPC();
#endif
}

#ifdef __linux__

static void cleanupDiscordIPCAtExit();

std::string DiscordService::getXDGRuntimeDir()
{
    const char *envVariable = std::getenv("XDG_RUNTIME_DIR");
    std::string runtimeDir;
    if (envVariable)
    {
        runtimeDir = std::string(envVariable);
        if (!runtimeDir.empty() && runtimeDir.back() == '/')
        {
            runtimeDir.pop_back();
        }
    }
    else
    {
        runtimeDir = "/run/user/" + std::to_string(getuid());
    }

    return runtimeDir;
}

std::vector<std::string> DiscordService::getIPCPaths(const std::string &runtimeDir, bool recursive)
{
    std::vector<std::string> ipcList;

    auto processEntry = [&](const std::filesystem::directory_entry &entry)
    {
        const auto &path = entry.path();
        std::string filename = path.filename().string();

        if (filename.rfind("discord-ipc-", 0) == 0)
        {
            ipcList.emplace_back(path.string());
        }
    };

    std::error_code ec;
    if (!recursive)
    {
        for (const auto &entry : std::filesystem::directory_iterator(runtimeDir, ec))
        {
            if (ec)
            {
                ec.clear();
                continue;
            }

            processEntry(entry);
        }
    }
    else
    {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(runtimeDir,
                                                                  std::filesystem::directory_options::skip_permission_denied,
                                                                  ec))
        {
            if (ec)
                continue;

            processEntry(entry);
        }
    }

    std::sort(ipcList.begin(), ipcList.end());
    return ipcList;
}

bool DiscordService::setupDiscordIPC()
{
    std::string runtimeDir = getXDGRuntimeDir();

    // Grants Discord IPC on Runtime Dir
    std::vector<std::string> ipcPaths = getIPCPaths(runtimeDir);
    for (const auto &ipc : ipcPaths)
    {
        struct stat statBuffer;
        if (lstat(ipc.c_str(), &statBuffer) != 0)
            continue;

        if (S_ISSOCK(statBuffer.st_mode))
        {
            return true;
        }
        else if (S_ISLNK(statBuffer.st_mode))
        {
            char linkTarget[1024];
            ssize_t len = readlink(ipc.c_str(), linkTarget, sizeof(linkTarget) - 1);

            if (len != -1)
            {
                linkTarget[len] = '\0';
                struct stat targetStatBuffer;
                if (stat(linkTarget, &targetStatBuffer) != 0)
                    continue;

                if (S_ISSOCK(targetStatBuffer.st_mode))
                {
                    std::cout << "Detected Discord IPC symlink. " << std::endl;
                    ipcSymlinkCreated = true;
                    ipcSymlinkPath = ipc;

                    static bool cleanupRegistered = false;
                    if (!cleanupRegistered)
                    {
                        std::atexit(cleanupDiscordIPCAtExit);
                        cleanupRegistered = true;
                    }
                    return true;
                }
                else
                {
                    std::cout << "Detected invalid Discord IPC symlink. Removing..." << std::endl;
                    unlink(ipc.c_str());
                }
            }
            else
            {
                std::cout << "Detected invalid Discord IPC symlink. Removing..." << std::endl;
                unlink(ipc.c_str());
            }
        }
    }

    // Find all IPC paths and try to create a symlink to the first valid one
    std::string standardIPC = runtimeDir + "/discord-ipc-0";
    unlink(standardIPC.c_str());
    ipcPaths = getIPCPaths(runtimeDir, true);

    for (const auto &ipc : ipcPaths)
    {
        struct stat statBuffer;
        if (stat(ipc.c_str(), &statBuffer) == 0 && S_ISSOCK(statBuffer.st_mode))
        {
            // Sanity check to avoid symlinking to the same path
            if (ipc == standardIPC)
                continue;

            std::cout << "Detected Alternative Discord IPC: " << ipc << std::endl;

            if (symlink(ipc.c_str(), standardIPC.c_str()) == 0)
            {
                std::cout << "Created Discord IPC symlink." << std::endl;
                ipcSymlinkCreated = true;
                ipcSymlinkPath = standardIPC;

                static bool cleanupRegistered = false;
                if (!cleanupRegistered)
                {
                    std::atexit(cleanupDiscordIPCAtExit);
                    cleanupRegistered = true;
                }

                return true;
            }
            else
            {
                std::cerr << "Failed to create symlink: " << strerror(errno) << std::endl;
                continue;
            }
        }
    }

    std::cout << "Warning: Discord IPC socket not found. Discord RPC may not work." << std::endl;

    return false;
}

void DiscordService::cleanupDiscordIPC()
{

    struct stat statbuf;

    if (ipcSymlinkCreated && !ipcSymlinkPath.empty())
    {
        std::cout << "Cleaning up Discord IPC symlink." << std::endl;

        if (lstat(ipcSymlinkPath.c_str(), &statbuf) == 0 && S_ISLNK(statbuf.st_mode))
        {
            unlink(ipcSymlinkPath.c_str());
        }

        ipcSymlinkCreated = false;
        ipcSymlinkPath.clear();
    }
}

static void cleanupDiscordIPCAtExit()
{
    auto instance = DiscordService::getInstance();
    if (instance)
    {
        instance->cleanupDiscordIPC();
    }
}
#endif // __linux__

bool DiscordService::initialize()
{

    // Check if already initialized
    if (core)
    {
        return true;
    }

#ifdef __linux__
    // Setup Discord IPC (detect and create symlink for alternative discord IPC paths)
    setupDiscordIPC();
#endif

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
                                                                } });
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

#ifdef __linux__
    // Clean up Discord IPC symlink if we created it
    cleanupDiscordIPC();
#endif

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