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
#include <iostream>
#include <sstream>
#include <type_traits>
#include <typeinfo>

#include "ConfigUtils.hpp"
#include "SettingsStructs.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

#ifdef _WIN32
#include <memory>
#endif

class Settings {
public:
  static Settings* Instance();
	void initSettings(const std::string& iniFolder);
  void saveSettings();
  std::string appFolder();
  bool isAllowedFileExtension(std::string fileExtension, const std::vector<std::string>& allowedExtensions = std::vector<std::string>()) const;
  bool isAllowedStyleExtension(std::string fileExtension) const;
  bool isAllowedImageExtension(std::string fileExtension) const;
  void setLogFunc(const std::function<void(std::string)>& doLog);
  void saveRecentFiles(const std::vector<FBEntity>& recentFiles);
  std::vector<FBEntity> loadRecentFiles();
  void saveRecentFilesImported(const std::vector<FBEntity>& recentFiles);
  std::vector<FBEntity> loadRecentFilesImported();
  void logTimings(const std::string& file, const std::string& method);

  unsigned short int OpenGL_MajorVersion, OpenGL_MinorVersion;
  bool maybeGracefullApplicationQuit;
  std::function<void(std::string)> funcDoLog;
  std::string ApplicationConfigurationFolder, appVersion, currentFolder, newLineDelimiter, SettingsFile, UIFontFile;
  bool wireframesMode, logDebugInfo, logFileBrowser, ShowBoundingBox, BoundingBoxRefresh, showPickRays;
  bool showPickRaysSingle, Terrain_HeightmapImageHistory, showAllVisualArtefacts;
  InAppRendererType RendererType;
  Color guiClearColor;
  int SDL_Window_Flags, SDL_Window_Width, SDL_Window_Height;
  int SDL_DrawableSize_Width, SDL_DrawableSize_Height;
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
	ImportExportFormats ImportExportFormat;
  bool showFrameRenderTime;
	bool shouldRecompileShaders, showGLErrors;
  bool grOcclusionCulling;

  void timerStart(const std::string& msg);
  void timerEnd(const std::string& msg);
  const bool hasEnding(std::string const &fullString, std::string const &ending) const;

  std::unique_ptr<KuplungApp::Utilities::GL::GLUtils> glUtils;
	const std::string getTimeNow() const;

	std::vector<std::string> hddDriveList;
	int Setting_SelectedDriveIndex, Setting_CurrentDriveIndex;

	std::vector<SupportedAssimpFormat> AssimpSupportedFormats_Import, AssimpSupportedFormats_Export;

  template <class T>
  inline void printClassAlignment(T *s) {
    std::cout << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
  }

  template <typename T>
  inline void printTypeAlignment(T s) {
    std::cout << "Size: " << sizeof(s) << " - " << typeid(s).name() << " <-> " << std::alignment_of<T>() << '\n';
  }

#ifdef _WIN32
  template <typename... Args>
  void string_format(const std::string& format, Args... args) {
    return snprintf(format, args);
  }

  template <typename... Args>
  std::string string_format_sprintf(const std::string& format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
  }
#else
  std::string string_format(const std::string& fmt_str, ...);
#endif

private:
  Settings(){}
  Settings(Settings const&){}
  Settings& operator=(Settings const&);
  static Settings* m_pInstance;

  std::unique_ptr<ConfigUtils> cfgUtils;
  void reuseLogFunc(const std::string& msg);
};

#endif /* Settings_h */
