#include <iostream>
#include <drogon/drogon.h>
#include "../third_party/discord-sdk-src/cpp/discord.h"

int main() {

    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0",2424);
    std::cout << "Hello, world!\n";
    //Load config file
    //drogon::app().loadConfigFile("../config.json");
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
