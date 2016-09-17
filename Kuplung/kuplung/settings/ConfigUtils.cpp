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

#pragma mark - Cleanup

ConfigUtils::~ConfigUtils() {
    this->configData = {};
}

#pragma mark - Init

void ConfigUtils::init(std::string const& appFolder) {
    this->configFile = appFolder + "/Kuplung_Settings.ini";
    this->recentFilesFile = appFolder + "/Kuplung_RecentFiles.ini";
    this->recentFilesFileImported = appFolder + "/Kuplung_RecentFilesImported.ini";
    this->regex_comment = "^#.*";
    this->regex_equalsSign = "=";
    this->readFile();
}

#pragma mark - Public

void ConfigUtils::saveSettings() {
#ifdef _WIN32
    std::string nlDelimiter = "\r\n";
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
    std::ofstream out(this->configFile);
    out << configLines;
    out.close();
}

#pragma mark - Read

bool ConfigUtils::readBool(std::string configKey) {
    return this->configData[configKey] == "true";
}

int ConfigUtils::readInt(std::string configKey) {
    return std::stoi(this->configData[configKey]);
}

float ConfigUtils::readFloat(std::string configKey) {
    return std::stof(this->configData[configKey]);
}

std::string ConfigUtils::readString(std::string configKey) {
    return this->configData[configKey];
}

#pragma mark - Write

void ConfigUtils::writeBool(std::string configKey, bool configValue) {
    this->configData[configKey] = configValue ? "true" : "false";
}

void ConfigUtils::writeInt(std::string configKey, int configValue) {
    this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeFloat(std::string configKey, float configValue) {
    this->configData[configKey] = std::to_string(configValue);
}

void ConfigUtils::writeString(std::string configKey, std::string configValue) {
    this->configData[configKey] = configValue;
}

void ConfigUtils::saveRecentFiles(std::vector<FBEntity> recentFiles) {
#ifdef _WIN32
        std::string nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
        std::string nlDelimiter = "\r";
#else
        std::string nlDelimiter = "\n";
#endif
    std::string recentFilesLines = "# Recent Files list" + nlDelimiter + nlDelimiter;
    for (size_t i=0; i<recentFiles.size(); i++) {
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
    std::FILE *fp = std::fopen(this->recentFilesFile.c_str(), "rb");
    if (fp) {
#ifdef _WIN32
        std::string nlDelimiter = "\r\n";
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

            if (singleLine == "" || std::regex_match(singleLine, this->regex_comment)) {
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

void ConfigUtils::saveRecentFilesImported(std::vector<FBEntity> recentFilesImported) {
#ifdef _WIN32
        std::string nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
        std::string nlDelimiter = "\r";
#else
        std::string nlDelimiter = "\n";
#endif
    std::string recentFilesLines = "# Recent Imported Files list" + nlDelimiter + nlDelimiter;
    for (size_t i=0; i<recentFilesImported.size(); i++) {
        FBEntity fileEntity = recentFilesImported[i];
        recentFilesLines += "# File" + nlDelimiter;
        recentFilesLines += fileEntity.title + nlDelimiter;
        recentFilesLines += fileEntity.path + nlDelimiter;
        recentFilesLines += nlDelimiter;
    }
    std::ofstream out(this->recentFilesFileImported);
    out << recentFilesLines;
    out.close();
}

std::vector<FBEntity> ConfigUtils::loadRecentFilesImported() {
    std::vector<FBEntity> recentFiles;
    std::FILE *fp = std::fopen(this->recentFilesFileImported.c_str(), "rb");
    if (fp) {
#ifdef _WIN32
        std::string nlDelimiter = "\r\n";
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

            if (singleLine == "" || std::regex_match(singleLine, this->regex_comment)) {
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

#pragma mark - Private

void ConfigUtils::readFile() {
    std::FILE *fp = std::fopen(this->configFile.c_str(), "rb");
    if (fp) {
#ifdef _WIN32
        std::string nlDelimiter = "\r\n";
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

            if (singleLine == "" || std::regex_match(singleLine, this->regex_comment)) {
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
                    opValue = "";
                this->configData[opKey] = opValue;
            }

            fileContents.erase(0, pos + nlDelimiter.length());
        }
    }
}

std::vector<std::string> ConfigUtils::splitString(const std::string &s, std::regex delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        elements.push_back(*iter);
    return elements;
}
