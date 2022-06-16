#pragma once

#include <thread>

class WebServerManager
{
private:
    static void onStop();
private:
    static std::thread *webServerThread;
public:
    static void onStart();
    static void start();
    static void stop();
};