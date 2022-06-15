#pragma once

#include <thread>

class WebServerManager
{
private:
    static void onStart();
    static void onStop();
private:
    static std::thread *webServerThread;
public:
    static void start();
    static void stop();
};