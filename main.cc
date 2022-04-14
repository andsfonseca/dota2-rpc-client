#include <iostream>
#include <drogon/drogon.h>
#include "../third_party/discord-sdk-src/cpp/discord.h"

enum ArgumentOptions
{
    NONE,
    HOST,
    PORT,
};

ArgumentOptions resolveArgumentOption(std::string input)
{
    if (input == "-h" || input == "--host")
        return HOST;
    if (input == "-p" || input == "--port")
        return PORT;
    return NONE;
}

int main(int argc, char *argv[])
{
    std::string host = "127.0.0.1";
    int port_number = 52424;

    if (argc > 1)
    {
        for (int i = 1; i < argc;)
        {
            switch (resolveArgumentOption(argv[i]))
            {
            case HOST:
                host = argv[i + 1];
                i = i + 2;
                break;
            case PORT:
                port_number = std::stoi(argv[i + 1]);
                i = i + 2;
                break;
            default:
                break;
            }
        }
    }

    // Set HTTP listener address and port
    drogon::app().addListener(host, port_number);

    // Load config file
    // drogon::app().loadConfigFile("../config.json");

    // Web Server Messages
    if (host == "0.0.0.0")
    {
        std::cout << "Listening at http://localhost:" << port_number << " and http://"<< host << ":" << port_number << ".\n";
    }
    else
    {
        std::cout << "Listening at http://"<< host << ":" << port_number << ".\n";
    }
    std::cout << "Press Ctrl+C to exit.\n";

    // Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
