//
//  Settings.h
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Settings_h
#define Settings_h

#include <string>
#include <vector>
#include "ConfigUtils.hpp"
#include "SettingsStructs.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Settings {
public:
    static Settings* Instance();
    void saveSettings();
    std::string string_format(const std::string fmt_str, ...);
    std::string appFolder();
    bool isAllowedFileExtension(std::string fileExtension);
    bool isAllowedStyleExtension(std::string fileExtension);
    bool isAllowedImageExtension(std::string fileExtension);
    void setLogFunc(std::function<void(std::string)> doLog);
    void saveRecentFiles(std::map <std::string, FBEntity> recentFiles);
    std::map <std::string, FBEntity> loadRecentFiles();

    std::function<void(std::string)> funcDoLog;
    std::string appVersion, currentFolder, newLineDelimiter, SettingsFile, UIFontFile;
    bool wireframesMode, logDebugInfo, logFileBrowser;
    Color guiClearColor;
    int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
    int frameLog_Width, frameLog_Height;
    int frameFileBrowser_Width, frameFileBrowser_Height;
    int UIFontFileIndex, ModelFileParser;
    float UIFontSize;
    int sceneCountObjects, sceneCountVertices, sceneCountIndices, sceneCountTriangles, sceneCountFaces;

private:
    Settings(){};
    Settings(Settings const&){};
    Settings& operator=(Settings const&);
    static Settings* m_pInstance;

    void initSettings();

    ConfigUtils *cfgUtils;
};

#endif /* Settings_h */
