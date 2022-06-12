#pragma once

#include "FileLoader.h"

class TemplateLoader : public FileLoader
{
public:
    static bool load(const std::string path, std::string &output);
};