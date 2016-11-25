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
#include <algorithm>
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
    void saveRecentFiles(std::vector<FBEntity> recentFiles);
    std::vector<FBEntity> loadRecentFiles();
    void saveRecentFilesImported(std::vector<FBEntity> recentFiles);
    std::vector<FBEntity> loadRecentFilesImported();

    std::function<void(std::string)> funcDoLog;
    std::string appVersion, currentFolder, newLineDelimiter, SettingsFile, UIFontFile;
    bool wireframesMode, logDebugInfo, logFileBrowser, ShowBoundingBox, BoundingBoxRefresh, showPickRays, showPickRaysSingle, Terrain_HeightmapImageHistory;
    InAppRendererType RendererType;
    Color guiClearColor;
    int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
    int frameLog_Width, frameLog_Height;
    int frameFileBrowser_Width, frameFileBrowser_Height;
    int UIFontFileIndex, ModelFileParser;
    float UIFontSize, BoundingBoxPadding;
    int sceneCountObjects, sceneCountVertices, sceneCountIndices, sceneCountTriangles, sceneCountFaces;
    int Consumption_Interval_CPU, Consumption_Interval_Memory;
    bool mRayDraw, mRayAnimate;
    float mRayOriginX, mRayOriginY, mRayOriginZ;
    float mRayDirectionX, mRayDirectionY, mRayDirectionZ;

    void timerStart(std::string msg);
    void timerEnd(std::string msg);

private:
    Settings(){};
    Settings(Settings const&){};
    Settings& operator=(Settings const&);
    static Settings* m_pInstance;

    void initSettings();
    std::unique_ptr<ConfigUtils> cfgUtils;
    std::string getTimeNow();
};

#endif /* Settings_h */
