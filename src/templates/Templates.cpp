#include <string>
#include <filesystem>
#include <fstream>

#include "../extensions/StringExtensions.cpp"

#if defined(_WIN32) || defined(_WIN64)
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace Templates
{

    static bool LoadFile(std::string path, std::string &fileAsString)
    {
        std::ifstream file(path);
        if (!file)
            return false;

        std::stringstream buffer;
        buffer << file.rdbuf();

        fileAsString = buffer.str();
        return true;
    }

    static std::string GetExecutableFolder()
    {
        std::string path;
#if defined(_WIN32) || defined(_WIN64)
        char result[MAX_PATH];
        path = std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
        Extensions::FindAndReplaceAll(path, "\\", "/");
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

    static bool LoadTemplate(std::string name, std::string &fileAsString){
        std::string folder = GetExecutableFolder();
        std::string path = folder + "/templates/" + name;

        return LoadFile(path, fileAsString);
    }

}