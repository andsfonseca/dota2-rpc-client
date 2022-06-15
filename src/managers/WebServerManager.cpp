#include <managers/WebServerManager.h>

#include <managers/ConfigurationManager.h>
#include <managers/LanguageManager.h>
#include <extensions/StringExtensions.h>

#include <iostream>

#include <drogon/drogon.h>

std::thread *WebServerManager::webServerThread = nullptr;

void WebServerManager::onStart()
{
    std::string host = ConfigurationManager::getHost();
    unsigned int port_number = ConfigurationManager::getPort();

    std::string listeningMessage;
    // Web Server Messages
    if (host == "0.0.0.0")
        listeningMessage = LanguageManager::getString("APP:INFO:SERVER_LISTENING_LOCALHOST",
                                                      LanguageManager::getSystemLanguage());
    else
        listeningMessage = LanguageManager::getString("APP:INFO:SERVER_LISTENING",
                                                      LanguageManager::getSystemLanguage());

    StringExtensions::findAndReplaceAll(listeningMessage,
                                        {"{{HOST}}", "{{PORT}}"},
                                        {host, std::to_string(port_number)});

    std::cout << listeningMessage << std::endl;
    std::cout << LanguageManager::getString("APP:INFO:SERVER_HOW_TO_EXIT", LanguageManager::getSystemLanguage()) << std::endl;

    // Set HTTP listener address and port
    drogon::app().addListener(host, port_number);

    drogon::app().run();
}

void WebServerManager::onStop()
{
    drogon::app().quit();
}

void WebServerManager::start()
{
    webServerThread = new std::thread(&onStart);
}

void WebServerManager::stop()
{
    onStop();
    webServerThread->join();
}