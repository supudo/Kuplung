//
//  ConfigUtils.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/4/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ConfigUtils_hpp
#define ConfigUtils_hpp

#include <map>
#include <regex>
#include <string>
#include <vector>
#include <map>
#include "SettingsStructs.h"

class ConfigUtils {
public:
    ~ConfigUtils();
    void init(std::string const& appFolder);
    void saveSettings();

    bool readBool(std::string configKey);
    int readInt(std::string configKey);
    float readFloat(std::string configKey);
    std::string readString(std::string configKey);

    void writeBool(std::string configKey, bool configValue);
    void writeInt(std::string configKey, int configValue);
    void writeFloat(std::string configKey, float configValue);
    void writeString(std::string configKey, std::string configValue);

    void saveRecentFiles(std::vector<FBEntity> recentFiles);
    std::vector<FBEntity> loadRecentFiles();

    void saveRecentFilesImported(std::vector<FBEntity> recentFilesImported);
    std::vector<FBEntity> loadRecentFilesImported();

private:
    std::string configFile, recentFilesFileImported, recentFilesFile;
    std::map<std::string, std::string> configData;

    std::regex regex_comment;
    std::regex regex_equalsSign;

    void readFile();
    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
};

#endif /* ConfigUtils_hpp */
