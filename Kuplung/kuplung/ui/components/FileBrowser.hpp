//
//  FileBrowser.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef FileBrowser_hpp
#define FileBrowser_hpp

#include "kuplung/settings/Settings.h"
#include <imgui.h>
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <filesystem>

class FileBrowser {
public:
  void init(bool log, int positionX, int positionY, int width, int height, const std::function<void(FBEntity, MaterialTextureType)>& processFile);
  void setStyleBrowser(const bool isStyle);
  void setImageBrowser(const bool isImage);
  void draw(const char* title, bool* p_opened = NULL, MaterialTextureType TextureType = MaterialTextureType_Undefined);

private:
  std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
  std::function<void(FBEntity, MaterialTextureType)> processFile;

  void drawFiles(const std::string& fPath, MaterialTextureType TextureType = MaterialTextureType_Undefined);
  std::string convertToString(double num);
  std::string convertSize(size_t size);
  double roundOff(double n);
  void logMessage(std::string const& logMessage);

  bool log, isStyleBrowser, isImageBrowser;
  int positionX, positionY, width, height;
	std::string currentFolder;
	float panelWidth_Options, panelWidth_OptionsMin;
};

#endif /* FileBrowser_hpp */
