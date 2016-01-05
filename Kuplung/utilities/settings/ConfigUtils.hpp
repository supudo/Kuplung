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

class ConfigUtils {
public:
    ~ConfigUtils();
    void init(std::string appFolder);
    void saveSettings();

    bool readBool(std::string configKey);
    int readInt(std::string configKey);
    float readFloat(std::string configKey);
    std::string readString(std::string configKey);

    void writeBool(std::string configKey, bool configValue);
    void writeInt(std::string configKey, int configValue);
    void writeFloat(std::string configKey, float configValue);
    void writeString(std::string configKey, std::string configValue);
    
private:
    std::string configFile;
    std::map<std::string, std::string> configData;
    
    std::regex regex_comment;
    std::regex regex_whiteSpace;
    std::regex regex_equalsSign;
    
    void readFile();
    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
};

#endif /* ConfigUtils_hpp */
