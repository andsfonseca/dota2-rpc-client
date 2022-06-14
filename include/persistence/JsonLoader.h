#pragma once

#include "FileLoader.h"
#include <json/json.h>

class JsonLoader : public FileLoader
{
public:
    static bool load(const std::string path, Json::Value &output);
    static bool save(const std::string path, Json::Value output);
    static Json::Value getNode(const Json::Value json, std::string key);
};