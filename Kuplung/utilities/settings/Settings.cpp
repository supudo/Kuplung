//
//  Settings.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Settings.h"
#include <SDL2/SDL.h>
#include <stdarg.h>
#include <memory>

#ifdef _WIN32
// ...
#else
#include "CoreFoundation/CoreFoundation.h"
#endif

#pragma mark - Singleton

Settings* Settings::m_pInstance = NULL;

Settings* Settings::Instance() {
    if (!m_pInstance) {
        m_pInstance = new Settings;
        m_pInstance->initSettings();
    }
    return m_pInstance;
}

#pragma mark - Init

void Settings::initSettings() {
    this->cfgUtils = new ConfigUtils();
    this->cfgUtils->init(Settings::Instance()->appFolder());

    m_pInstance->OpenGLMajorVersion = m_pInstance->cfgUtils->readInt("OpenGLMajorVersion");
    m_pInstance->OpenGLMinorVersion = m_pInstance->cfgUtils->readInt("OpenGLMinorVersion");
    m_pInstance->OpenGL_GLSL_Version = m_pInstance->cfgUtils->readInt("OpenGL_GLSL_Version");

    m_pInstance->appVersion = m_pInstance->cfgUtils->readString("appVersion");
    m_pInstance->currentFolder = m_pInstance->cfgUtils->readString("currentFolder");
    m_pInstance->ShaderName = m_pInstance->cfgUtils->readString("ShaderName");

    m_pInstance->showGrid = m_pInstance->cfgUtils->readBool("showGrid");
    m_pInstance->showLight = m_pInstance->cfgUtils->readBool("showLight");
    m_pInstance->showAxes = m_pInstance->cfgUtils->readBool("showAxes");
    m_pInstance->wireframesMode = m_pInstance->cfgUtils->readBool("wireframesMode");
    m_pInstance->logDebugInfo = m_pInstance->cfgUtils->readBool("logDebugInfo");
    m_pInstance->logFileBrowser = m_pInstance->cfgUtils->readBool("logFileBrowser");

    m_pInstance->SDL_Window_Width = m_pInstance->cfgUtils->readInt("SDL_Window_Width");
    m_pInstance->SDL_Window_Height = m_pInstance->cfgUtils->readInt("SDL_Window_Height");

    m_pInstance->frameLog_Width = m_pInstance->cfgUtils->readInt("frameLog_Width");
    m_pInstance->frameLog_Height = m_pInstance->cfgUtils->readInt("frameLog_Height");

    m_pInstance->frameFileBrowser_Width = m_pInstance->cfgUtils->readInt("frameFileBrowser_Width");
    m_pInstance->frameFileBrowser_Height = m_pInstance->cfgUtils->readInt("frameFileBrowser_Height");

    m_pInstance->guiClearColor = {70.0 / 255.0, 70.0 / 255.0, 70.0 / 255.0, 255.0 / 255.0};
    m_pInstance->SDL_Window_Flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

#ifdef _WIN32
    m_pInstance->newLineDelimiter = "\r\n";
#elif defined macintosh // OS 9
    m_pInstance->newLineDelimiter = "\r";
#else
    m_pInstance->newLineDelimiter = "\n";
#endif
}

#pragma mark - Public

std::string Settings::appFolder() {
#ifdef _WIN32
    //TODO: Windows
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
    this->cfgUtils->writeInt("OpenGLMajorVersion", this->OpenGLMajorVersion);
    this->cfgUtils->writeInt("OpenGLMinorVersion", this->OpenGLMinorVersion);
    this->cfgUtils->writeInt("OpenGL_GLSL_Version", this->OpenGL_GLSL_Version);

    this->cfgUtils->writeString("appVersion", this->appVersion);
    this->cfgUtils->writeString("currentFolder", this->currentFolder);
    this->cfgUtils->writeString("ShaderName", this->ShaderName);

    this->cfgUtils->writeBool("showGrid", this->showGrid);
    this->cfgUtils->writeBool("showLight", this->showLight);
    this->cfgUtils->writeBool("showAxes", this->showAxes);
    this->cfgUtils->writeBool("wireframesMode", this->wireframesMode);
    this->cfgUtils->writeBool("logDebugInfo", this->logDebugInfo);
    this->cfgUtils->writeBool("logFileBrowser", this->logFileBrowser);

    this->cfgUtils->writeInt("frameLog_Width", this->frameLog_Width);
    this->cfgUtils->writeInt("frameLog_Height", this->frameLog_Height);

    this->cfgUtils->writeInt("frameFileBrowser_Width", this->frameFileBrowser_Width);
    this->cfgUtils->writeInt("frameFileBrowser_Height", this->frameFileBrowser_Height);

    this->cfgUtils->writeInt("SDL_Window_Width", this->SDL_Window_Width);
    this->cfgUtils->writeInt("SDL_Window_Height", this->SDL_Window_Height);

    this->cfgUtils->saveSettings();
}

std::string Settings::string_format(const std::string fmt_str, ...) {
    int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
    std::string str;
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

bool Settings::isAllowedFileExtension(std::string fileExtension) {
    std::string allowedExtensions[] = {".obj"};
    return std::find(std::begin(allowedExtensions), std::end(allowedExtensions), fileExtension) != std::end(allowedExtensions);
}
