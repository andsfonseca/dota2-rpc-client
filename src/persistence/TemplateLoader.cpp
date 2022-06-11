#include <persistence/TemplateLoader.h>

bool TemplateLoader::load(const std::string path, std::string &output)
{
    const std::string folder = getApplicationFolder();
    return FileLoader::load(folder + "/templates/" + path, output);
}