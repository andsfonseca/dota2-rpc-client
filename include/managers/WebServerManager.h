#pragma once

#include <thread>

class WebServerManager
{
private:
    static std::thread *webServerThread;
public:
    static void onStart();
    static void onStop();
    static void start();
    static void stop();
};