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
#include <iostream>
#include <type_traits>
#include <typeinfo>

#include "ConfigUtils.hpp"
#include "SettingsStructs.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

#ifdef WIN32
#include <memory>
#endif

#define BOOST_FILESYSTEM_NO_DEPRECATED

class Settings {
public:
    static Settings* Instance();
    void saveSettings();
    std::string string_format(const std::string fmt_str, ...);
    std::string appFolder();
    bool isAllowedFileExtension(std::string fileExtension, const std::vector<std::string>& allowedExtensions = std::vector<std::string>());
    bool isAllowedStyleExtension(std::string fileExtension);
    bool isAllowedImageExtension(std::string fileExtension);
    void setLogFunc(const std::function<void(std::string)>& doLog);
    void saveRecentFiles(const std::vector<FBEntity>& recentFiles);
    std::vector<FBEntity> loadRecentFiles();
    void saveRecentFilesImported(const std::vector<FBEntity>& recentFiles);
    std::vector<FBEntity> loadRecentFilesImported();

    bool maybeGracefullApplicationQuit;
    std::function<void(std::string)> funcDoLog;
    std::string appVersion, currentFolder, newLineDelimiter, SettingsFile, UIFontFile;
    bool wireframesMode, logDebugInfo, logFileBrowser, ShowBoundingBox, BoundingBoxRefresh, showPickRays;
    bool showPickRaysSingle, Terrain_HeightmapImageHistory, showAllVisualArtefacts;
    InAppRendererType RendererType;
    Color guiClearColor;
    int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
    int frameLog_Width, frameLog_Height;
    int frameFileBrowser_Width, frameFileBrowser_Height;
    int UIFontFileIndex, ModelFileParser;
    int GUISystem;
    float UIFontSize, BoundingBoxPadding;
    int sceneCountObjects, sceneCountVertices, sceneCountIndices, sceneCountTriangles, sceneCountFaces;
    int Consumption_Interval_CPU, Consumption_Interval_Memory;
    bool mRayDraw, mRayAnimate;
    float mRayOriginX, mRayOriginY, mRayOriginZ;
    float mRayDirectionX, mRayDirectionY, mRayDirectionZ;
    bool UseCuda;

    void timerStart(const std::string& msg);
    void timerEnd(const std::string& msg);
    bool hasEnding(std::string const &fullString, std::string const &ending);

    std::unique_ptr<KuplungApp::Utilities::GL::GLUtils> glUtils;

    template <class T>
    inline void printClassAlignment(T *s) {
        std::cout << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
    }

    template <typename T>
    inline void printTypeAlignment(T s) {
        std::cout << "Size: " << sizeof(s) << " - " << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
    }

private:
    Settings(){}
    Settings(Settings const&){}
    Settings& operator=(Settings const&);
    static Settings* m_pInstance;

    void initSettings();
    std::unique_ptr<ConfigUtils> cfgUtils;
    std::string getTimeNow();
    void reuseLogFunc(const std::string& msg);
};

#endif /* Settings_h */
