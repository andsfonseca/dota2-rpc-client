#include <persistence/JsonLoader.h>
#include <iostream>

bool JsonLoader::load(const std::string path, Json::Value &output)
{
    std::string aux;

    if (!FileLoader::load(path, aux))
        return false;

    Json::Reader reader;

    if (!reader.parse(aux, output))
    {
        std::cerr << "Corrupted Json File" << std::endl;
        return false;
    }

    return true;
}

bool JsonLoader::save(const std::string path, Json::Value output)
{
    Json::StreamWriterBuilder wbuilder;
    wbuilder["indentation"] = "\t";
    
    return FileLoader::save(path, Json::writeString(wbuilder, output));
}

Json::Value JsonLoader::getNode(const Json::Value json, std::string key)
{

    Json::Value child = json;

    if (key == "" || child == NULL)
        return NULL;

    size_t aux = key.find(':');
    std::string token = key.substr(0, aux);
    key.erase(0, aux + 1);
    std::string lastToken = "";

    while (key != "" && token != key)
    {
        if (child[token].isNull())
            return NULL;

        child = child[token];

        lastToken = token;
        aux = key.find(':');
        token = key.substr(0, aux);
        key.erase(0, aux + 1);
    }
    return child[token];
}