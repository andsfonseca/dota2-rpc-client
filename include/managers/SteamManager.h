#pragma once

#include <string>

class SteamManager
{
private:
    static std::string getSteamPath();
    static std::string showSelectFolderDialog();
    static std::string generateDota2CFG();
    static void showCannotCreateFileWarning(); 
    static bool findDota2PathUsingVDF(const std::string vdfPath, std::string &dota2path);
public:
    static void onInstall();
    static void onStart();
};