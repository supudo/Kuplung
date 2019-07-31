//
//  ConfigUtils.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/4/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ConfigUtils_hpp
#define ConfigUtils_hpp

#include "SettingsStructs.h"
#include <map>
#include <regex>
#include <string>
#include <vector>

class ConfigUtils {
public:
  ~ConfigUtils();
  void init(std::string const& appFolder);
  void saveSettings();

  bool readBool(std::string const& configKey);
  int readInt(std::string const& configKey);
  float readFloat(std::string const& configKey);
  std::string readString(std::string const& configKey);

  void writeBool(std::string const& configKey, bool const& configValue);
  void writeInt(std::string const& configKey, int const& configValue);
  void writeFloat(std::string const& configKey, float const& configValue);
  void writeString(std::string const& configKey, std::string const& configValue);

  void saveRecentFiles(std::vector<FBEntity> const& recentFiles);
  std::vector<FBEntity> loadRecentFiles();

  void saveRecentFilesImported(std::vector<FBEntity> const& recentFilesImported);
  std::vector<FBEntity> loadRecentFilesImported();

private:
  std::string configFile, recentFilesFileImported, recentFilesFile;
  std::map<std::string, std::string> configData;

  std::regex regex_equalsSign;

  void readFile();
  std::vector<std::string> splitString(const std::string& s, std::regex const& delimiter);
};

#endif /* ConfigUtils_hpp */
