//
//  ConfigUtils.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/4/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ConfigUtils.hpp"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <mutex>
#include <stdexcept>

ConfigUtils::~ConfigUtils() {
  this->configData = {};
}

void ConfigUtils::init(std::string const& appFolder) {
  this->configFile = appFolder + "/Kuplung_Settings.ini";
  this->recentFilesFile = appFolder + "/Kuplung_RecentFiles.ini";
  this->recentFilesFileImported = appFolder + "/Kuplung_RecentFilesImported.ini";
  this->regex_equalsSign = "=";
  this->readFile();
}

void ConfigUtils::saveSettings() {
#ifdef _WIN32
  std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
  std::string nlDelimiter = "\r";
#else
  std::string nlDelimiter = "\n";
#endif
  std::string configLines("");
  for (std::map<std::string, std::string>::iterator iter = this->configData.begin(); iter != this->configData.end(); ++iter) {
    std::string cfgKey = iter->first;
    std::string cfgValue = iter->second;
    configLines += cfgKey + " = " + cfgValue + nlDelimiter;
  }
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);

  std::ofstream out(this->configFile);
  if (!out.is_open())
    throw std::runtime_error("[Kuplung] Unable to save settings file!");
  out << configLines << std::endl;
  out.close();
}

bool ConfigUtils::readBool(std::string const& configKey) {
  return this->configData[configKey] == "true";
}

int ConfigUtils::readInt(std::string const& configKey) {
  std::string val = this->configData[configKey];
  if (!val.empty())
    return std::stoi(val);
  else
    return 0;
}

float ConfigUtils::readFloat(std::string const& configKey) {
  std::string val = this->configData[configKey];
  if (!val.empty())
    return std::stof(val);
  else
    return 0;
}

std::string ConfigUtils::readString(std::string const& configKey) {
  return this->configData[configKey];
}

void ConfigUtils::writeBool(std::string const& configKey, bool const& configValue) {
  this->configData[configKey] = configValue ? "true" : "false";
}

void ConfigUtils::writeInt(std::string const& configKey, int const& configValue) {
  this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeFloat(std::string const& configKey, float const& configValue) {
  this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeString(std::string const& configKey, std::string const& configValue) {
  this->configData[configKey] = configValue;
}

void ConfigUtils::saveRecentFiles(std::vector<FBEntity> const& recentFiles) {
#ifdef _WIN32
  std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
  std::string nlDelimiter = "\r";
#else
  std::string nlDelimiter = "\n";
#endif
  std::string recentFilesLines = "# Recent Files list" + nlDelimiter + nlDelimiter;
  for (size_t i = 0; i < recentFiles.size(); i++) {
    FBEntity fileEntity = recentFiles[i];
    recentFilesLines += "# File" + nlDelimiter;
    recentFilesLines += fileEntity.title + nlDelimiter;
    recentFilesLines += fileEntity.path + nlDelimiter;
    recentFilesLines += nlDelimiter;
  }
  std::ofstream out(this->recentFilesFile);
  out << recentFilesLines;
  out.close();
}

std::vector<FBEntity> ConfigUtils::loadRecentFiles() {
  std::vector<FBEntity> recentFiles;
  std::FILE* fp = std::fopen(this->recentFilesFile.c_str(), "rb");
  if (fp) {
#ifdef _WIN32
    std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
    std::string nlDelimiter = "\r";
#else
    std::string nlDelimiter = "\n";
#endif

    std::string fileContents;
    std::fseek(fp, 0, SEEK_END);
    fileContents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&fileContents[0], 1, fileContents.size(), fp);
    std::fclose(fp);

    size_t pos = 0;
    std::string singleLine;

    recentFiles = {};

    int fileCounter = 0;
    FBEntity fileEntity;
    while ((pos = fileContents.find(nlDelimiter)) != std::string::npos) {
      singleLine = fileContents.substr(0, pos);

      if (singleLine.empty() || singleLine.at(0) == '#' || singleLine.at(0) == '\n' || singleLine.at(0) == '\r' || singleLine.at(0) == '\r\n') {
        fileContents.erase(0, pos + nlDelimiter.length());
        fileCounter = 0;
        continue;
      }
      else {
        if (fileCounter == 0)
          fileEntity.title = singleLine;
        else if (fileCounter == 1) {
          fileEntity.isFile = true;
          fileEntity.path = singleLine;

          std::vector<std::string> elems;
          boost::split(elems, fileEntity.path, boost::is_any_of("."));
          fileEntity.extension = elems[elems.size() - 1];

          recentFiles.push_back(fileEntity);
        }
        fileCounter += 1;
      }

      fileContents.erase(0, pos + nlDelimiter.length());
    }
  }
  return recentFiles;
}

void ConfigUtils::saveRecentFilesImported(std::vector<FBEntity> const& recentFilesImported) {
#ifdef _WIN32
  std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
  std::string nlDelimiter = "\r";
#else
  std::string nlDelimiter = "\n";
#endif
  std::string recentFilesLines = "# Recent Imported Files list" + nlDelimiter + nlDelimiter;
  for (size_t i = 0; i < recentFilesImported.size(); i++) {
    FBEntity fileEntity = recentFilesImported[i];
    recentFilesLines += "# File" + nlDelimiter;
    recentFilesLines += (fileEntity.title.empty() ? "-" : fileEntity.title) + nlDelimiter;
    recentFilesLines += (fileEntity.path.empty() ? "-" : fileEntity.path) + nlDelimiter;
    recentFilesLines += nlDelimiter;
  }
  std::ofstream out(this->recentFilesFileImported, std::ios_base::trunc);
  out << recentFilesLines;
  out.close();
}

std::vector<FBEntity> ConfigUtils::loadRecentFilesImported() {
  std::vector<FBEntity> recentFiles;
  std::FILE* fp = std::fopen(this->recentFilesFileImported.c_str(), "rb");
  if (fp) {
#ifdef _WIN32
    std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
    std::string nlDelimiter = "\r";
#else
    std::string nlDelimiter = "\n";
#endif

    std::string fileContents;
    std::fseek(fp, 0, SEEK_END);
    fileContents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&fileContents[0], 1, fileContents.size(), fp);
    std::fclose(fp);

    size_t pos = 0;
    std::string singleLine;

    recentFiles = {};

    int fileCounter = 0;
    FBEntity fileEntity;
    while ((pos = fileContents.find(nlDelimiter)) != std::string::npos) {
      singleLine = fileContents.substr(0, pos);

      if (singleLine.empty() || singleLine.at(0) == '#' || singleLine.at(0) == '\n' || singleLine.at(0) == '\r' || singleLine.at(0) == '\r\n') {
        fileContents.erase(0, pos + nlDelimiter.length());
        fileCounter = 0;
        continue;
      }
      else {
        if (fileCounter == 0)
          fileEntity.title = singleLine;
        else if (fileCounter == 1) {
          fileEntity.isFile = true;
          fileEntity.path = singleLine;

          std::vector<std::string> elems;
          boost::split(elems, fileEntity.path, boost::is_any_of("."));
          fileEntity.extension = elems[elems.size() - 1];

          recentFiles.push_back(fileEntity);
        }
        fileCounter += 1;
      }

      fileContents.erase(0, pos + nlDelimiter.length());
    }
  }
  return recentFiles;
}

void ConfigUtils::readFile() {
  std::FILE* fp = std::fopen(this->configFile.c_str(), "rb");
  if (fp) {
#ifdef _WIN32
    std::string nlDelimiter = "\n";
#elif defined macintosh // OS 9
    std::string nlDelimiter = "\r";
#else
    std::string nlDelimiter = "\n";
#endif

    std::string fileContents;
    std::fseek(fp, 0, SEEK_END);
    fileContents.resize(std::ftell(fp));
    std::rewind(fp);
    std::fread(&fileContents[0], 1, fileContents.size(), fp);
    std::fclose(fp);

    size_t pos = 0;
    std::string singleLine;

    this->configData = {};

    while ((pos = fileContents.find(nlDelimiter)) != std::string::npos) {
      singleLine = fileContents.substr(0, pos);

      if (singleLine.empty() || singleLine.at(0) == '#' || singleLine.at(0) == '\n' || singleLine.at(0) == '\r' || singleLine.at(0) == '\r\n') {
        fileContents.erase(0, pos + nlDelimiter.length());
        continue;
      }
      else {
        std::string opKey, opValue;
        std::vector<std::string> lineElements = this->splitString(singleLine, this->regex_equalsSign);
        opKey = lineElements[0];
        boost::algorithm::trim(opKey);

        if (lineElements.size() > 1) {
          opValue = lineElements[1];
          boost::algorithm::trim(opValue);
        }
        else
          opValue.clear();
        this->configData[opKey] = opValue;
      }

      fileContents.erase(0, pos + nlDelimiter.length());
    }
  }
}

const std::vector<std::string> ConfigUtils::splitString(const std::string& s, std::regex const& delimiter) const {
  std::vector<std::string> elements;
  std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    elements.push_back(*iter);
  return elements;
}
