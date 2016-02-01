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
#include "ConfigUtils.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED

struct Color {
    float r, g, b, w;
};

struct FBEntity {
    bool isFile;
    std::string path, title, extension, modifiedDate, size;
};

class Settings {
public:
    static Settings* Instance();
    void saveSettings();
    std::string string_format(const std::string fmt_str, ...);
    std::string appFolder();
    bool isAllowedFileExtension(std::string fileExtension);
    bool isAllowedStyleExtension(std::string fileExtension);

    int OpenGLMajorVersion, OpenGLMinorVersion, OpenGL_GLSL_Version;
    std::string appVersion, currentFolder, newLineDelimiter, ShaderName, SettingsFile;
    bool showGrid, showLight, showAxes, wireframesMode, logDebugInfo, logFileBrowser;
    Color guiClearColor;
    int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
    int frameLog_Width, frameLog_Height;
    int frameFileBrowser_Width, frameFileBrowser_Height;

private:
    Settings(){};
    Settings(Settings const&){};
    Settings& operator=(Settings const&);
    static Settings* m_pInstance;

    void initSettings();

    ConfigUtils *cfgUtils;
};

#endif /* Settings_h */
