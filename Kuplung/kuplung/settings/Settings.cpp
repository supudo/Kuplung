//
//  Settings.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Settings.h"
#include <SDL2/SDL.h>
#ifdef _WIN32
#undef main
#include <boost/filesystem.hpp>
#include <iostream>
#else
#include "CoreFoundation/CoreFoundation.h"
#endif
#include <stdarg.h>
#include <memory>

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
    if (!m_pInstance) {
        m_pInstance = new Settings;
        m_pInstance->initSettings();
    }
    return m_pInstance;
}

void Settings::initSettings() {
    m_pInstance->mRayDraw = false;
    m_pInstance->mRayAnimate = false;
    m_pInstance->mRayOriginX = 0.0f;
    m_pInstance->mRayOriginY = 0.0f;
    m_pInstance->mRayOriginZ = 0.0f;
    m_pInstance->mRayDirectionX = 0.0f;
    m_pInstance->mRayDirectionY = 0.0f;
    m_pInstance->mRayDirectionZ = 0.0f;

    m_pInstance->cfgUtils = std::make_unique<ConfigUtils>();
    m_pInstance->cfgUtils->init(Settings::Instance()->appFolder());

    m_pInstance->appVersion = m_pInstance->cfgUtils->readString("appVersion");
    m_pInstance->currentFolder = m_pInstance->cfgUtils->readString("currentFolder");
    m_pInstance->UIFontFile = m_pInstance->cfgUtils->readString("UIFontFile");
    m_pInstance->UIFontSize = m_pInstance->cfgUtils->readInt("UIFontSize");
    m_pInstance->ModelFileParser = m_pInstance->cfgUtils->readInt("ModelFileParser");
    if (m_pInstance->ModelFileParser >= Importer_ParserType_Count)
        m_pInstance->ModelFileParser = Importer_ParserType_Own;
    m_pInstance->RendererType = static_cast<InAppRendererType>(m_pInstance->cfgUtils->readInt("RendererType"));
    m_pInstance->GUISystem = m_pInstance->cfgUtils->readInt("GUISystem");

    m_pInstance->wireframesMode = m_pInstance->cfgUtils->readBool("wireframesMode");
    m_pInstance->logDebugInfo = m_pInstance->cfgUtils->readBool("logDebugInfo");
    m_pInstance->logFileBrowser = m_pInstance->cfgUtils->readBool("logFileBrowser");
    m_pInstance->showPickRays = m_pInstance->cfgUtils->readBool("showPickRays");
    m_pInstance->showPickRaysSingle = m_pInstance->cfgUtils->readBool("showPickRaysSingle");
    m_pInstance->Terrain_HeightmapImageHistory = m_pInstance->cfgUtils->readBool("Terrain_HeightmapImageHistory");
#ifdef DEF_KuplungSetting_UseCuda
    m_pInstance->UseCuda = m_pInstance->cfgUtils->readBool("UseCuda");
#else
    m_pInstance->UseCuda = false;
#endif

    m_pInstance->SDL_Window_Width = m_pInstance->cfgUtils->readInt("SDL_Window_Width");
    m_pInstance->SDL_Window_Height = m_pInstance->cfgUtils->readInt("SDL_Window_Height");

    m_pInstance->ShowBoundingBox = m_pInstance->cfgUtils->readBool("ShowBoundingBox");
    m_pInstance->BoundingBoxPadding = m_pInstance->cfgUtils->readFloat("BoundingBoxPadding");
    m_pInstance->BoundingBoxRefresh = false;

    m_pInstance->frameLog_Width = m_pInstance->cfgUtils->readInt("frameLog_Width");
    m_pInstance->frameLog_Height = m_pInstance->cfgUtils->readInt("frameLog_Height");

    m_pInstance->frameFileBrowser_Width = m_pInstance->cfgUtils->readInt("frameFileBrowser_Width");
    m_pInstance->frameFileBrowser_Height = m_pInstance->cfgUtils->readInt("frameFileBrowser_Height");

    m_pInstance->Consumption_Interval_CPU = m_pInstance->cfgUtils->readInt("Consumption_Interval_CPU");
    m_pInstance->Consumption_Interval_Memory = m_pInstance->cfgUtils->readInt("Consumption_Interval_Memory");

    m_pInstance->guiClearColor = {70.0f / 255.0f, 70.0f / 255.0f, 70.0f / 255.0f, 255.0f / 255.0f};
    m_pInstance->SDL_Window_Flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    m_pInstance->UIFontFileIndex = 0;

    m_pInstance->sceneCountObjects = 0;
    m_pInstance->sceneCountVertices = 0;
    m_pInstance->sceneCountIndices = 0;
    m_pInstance->sceneCountTriangles = 0;
    m_pInstance->sceneCountFaces = 0;

    m_pInstance->showAllVisualArtefacts = true;
    m_pInstance->maybeGracefullApplicationQuit = false;

#ifdef _WIN32
    m_pInstance->newLineDelimiter = "\r\n";
#elif defined macintosh // OS 9
    m_pInstance->newLineDelimiter = "\r";
#else
    m_pInstance->newLineDelimiter = "\n";
#endif

    m_pInstance->glUtils = std::make_unique<KuplungApp::Utilities::GL::GLUtils>(std::bind(&Settings::reuseLogFunc, this, std::placeholders::_1));
}

void Settings::reuseLogFunc(const std::string& msg) {
    m_pInstance->funcDoLog(msg);
}

#pragma mark - Public

std::string Settings::appFolder() {
#ifdef _WIN32
    return boost::filesystem::current_path().string() + "/resources";
#else
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char folder[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)folder, PATH_MAX))
        printf("Can't open bundle folder!\n");
    CFRelease(resourcesURL);
    return std::string(folder);
#endif
}

void Settings::saveSettings() {
    this->cfgUtils->writeString("appVersion", this->appVersion);
    this->cfgUtils->writeString("currentFolder", this->currentFolder);
    this->cfgUtils->writeString("UIFontFile", this->UIFontFile);

    this->cfgUtils->writeInt("UIFontSize", static_cast<int>(this->UIFontSize));
    this->cfgUtils->writeInt("ModelFileParser", this->ModelFileParser);
    this->cfgUtils->writeInt("RendererType", this->RendererType);
    this->cfgUtils->writeInt("GUISystem", this->GUISystem);

    this->cfgUtils->writeBool("wireframesMode", this->wireframesMode);
    this->cfgUtils->writeBool("logDebugInfo", this->logDebugInfo);
    this->cfgUtils->writeBool("logFileBrowser", this->logFileBrowser);
    this->cfgUtils->writeBool("showPickRays", this->showPickRays);
    this->cfgUtils->writeBool("showPickRaysSingle", this->showPickRaysSingle);
    this->cfgUtils->writeBool("Terrain_HeightmapImageHistory", this->Terrain_HeightmapImageHistory);
    this->cfgUtils->writeBool("UseCuda", this->UseCuda);

    this->cfgUtils->writeBool("ShowBoundingBox", this->ShowBoundingBox);
    this->cfgUtils->writeFloat("BoundingBoxPadding", this->BoundingBoxPadding);

    this->cfgUtils->writeInt("frameLog_Width", this->frameLog_Width);
    this->cfgUtils->writeInt("frameLog_Height", this->frameLog_Height);

    this->cfgUtils->writeInt("frameFileBrowser_Width", this->frameFileBrowser_Width);
    this->cfgUtils->writeInt("frameFileBrowser_Height", this->frameFileBrowser_Height);

    this->cfgUtils->writeInt("SDL_Window_Width", this->SDL_Window_Width);
    this->cfgUtils->writeInt("SDL_Window_Height", this->SDL_Window_Height);

    this->cfgUtils->writeInt("Consumption_Interval_CPU", this->Consumption_Interval_CPU);
    this->cfgUtils->writeInt("Consumption_Interval_Memory", this->Consumption_Interval_Memory);

    this->cfgUtils->saveSettings();
}

std::string Settings::string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::unique_ptr<char[]> formatted;
    va_list ap;
    while(1) {
        formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
        strcpy(&formatted[0], fmt_str.c_str());
        va_start(ap, fmt_str);
        final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
        va_end(ap);
        if (final_n < 0 || final_n >= n)
            n += abs(final_n - n + 1);
        else
            break;
    }
    return std::string(formatted.get());
}

bool Settings::isAllowedFileExtension(std::string fileExtension, const std::vector<std::string>& allowedExtensions) {
    if (allowedExtensions.size() > 0) {
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
        return std::find(std::begin(allowedExtensions), std::end(allowedExtensions), fileExtension) != std::end(allowedExtensions);
    }
    else
        return true;
}

bool Settings::isAllowedStyleExtension(std::string fileExtension) {
    std::string allowedExtensions[] = {".style"};
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    return std::find(std::begin(allowedExtensions), std::end(allowedExtensions), fileExtension) != std::end(allowedExtensions);
}

bool Settings::isAllowedImageExtension(std::string fileExtension) {
    std::string allowedExtensions[] = {".png", ".jpg", ".bmp", ".jpeg", ".tga"};
    std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    return std::find(std::begin(allowedExtensions), std::end(allowedExtensions), fileExtension) != std::end(allowedExtensions);
}

void Settings::setLogFunc(const std::function<void(std::string)>& doLog) {
    this->funcDoLog = doLog;
}

void Settings::saveRecentFiles(const std::vector<FBEntity>& recentFiles) {
    this->cfgUtils->saveRecentFiles(recentFiles);
}

std::vector<FBEntity> Settings::loadRecentFiles() {
    return this->cfgUtils->loadRecentFiles();
}

void Settings::saveRecentFilesImported(const std::vector<FBEntity>& recentFiles) {
    this->cfgUtils->saveRecentFilesImported(recentFiles);
}

std::vector<FBEntity> Settings::loadRecentFilesImported() {
    return this->cfgUtils->loadRecentFilesImported();
}

void Settings::timerStart(const std::string& msg) {
    this->funcDoLog(this->string_format("[Timer START] %s - %s", msg.c_str(), this->getTimeNow().c_str()));
}

void Settings::timerEnd(const std::string& msg) {
    this->funcDoLog(this->string_format("[Timer END] %s - %s", msg.c_str(), this->getTimeNow().c_str()));
}

std::string Settings::getTimeNow() {
    time_t t = time(0);
    struct tm * now = localtime(&t);
    int t_hour = int(now->tm_hour);
    int t_min = int(now->tm_min);
    int t_sec = int(now->tm_sec);
    std::string tn("");
    if (t_hour < 10)
        tn += "0";
    tn += std::to_string(t_hour) + ":";
    if (t_min < 10)
        tn += "0";
    tn += std::to_string(t_min) + ":";
    if (t_sec < 10)
        tn += "0";
    tn += std::to_string(t_sec);
    return tn;
}

bool Settings::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    return false;
}

