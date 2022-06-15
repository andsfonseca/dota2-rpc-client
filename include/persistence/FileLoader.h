#pragma once

#include <string>

class FileLoader
{
public:
    static bool load(const std::string path, std::string &output);
    static bool save(const std::string path, const std::string &output);
    static std::string getApplicationFolder();
};