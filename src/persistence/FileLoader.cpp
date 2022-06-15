#include <persistence/FileLoader.h>
#include <extensions/StringExtensions.h>

#include <iostream>
#include <fstream>
#include <sstream>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif


bool FileLoader::load(const std::string path, std::string &output)
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "File '"+ path +" 'not found!" << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    output = buffer.str();
    return true;
}

bool FileLoader::save(const std::string path, const std::string &output)
{
    std::ofstream file(path);
    if(file.is_open())
    {
        file << output;
        file.close();
        return true;
    }
    return false;
}

std::string FileLoader::getApplicationFolder()
{
    std::string path;
#if defined(_WIN32) || defined(_WIN64)
    char result[MAX_PATH];
    path = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
    StringExtensions::findAndReplaceAll(path, "\\", "/");
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    path = std::string(result, (count > 0) ? count : 0);
#endif
    size_t last_slash_idx = path.rfind('/');
    if (std::string::npos != last_slash_idx)
    {
        path = path.substr(0, last_slash_idx);
    }
    return path;
}
