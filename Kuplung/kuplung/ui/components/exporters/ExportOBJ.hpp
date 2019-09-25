//
//  ExportOBJ.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ExportOBJ_hpp
#define ExportOBJ_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

class ExportOBJ {
public:
  void init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, std::vector<std::string>)>& saveFile);
  void draw(const char* title, bool* p_opened = nullptr);

private:
  std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
  std::function<void(FBEntity, std::vector<std::string>)> funcFileSave;

  void drawFiles(const std::string& fPath);
  const std::string convertToString(double num) const;
  const std::string convertSize(size_t size) const;
  const double roundOff(double n) const;
  void modalNewFolder();
  const bool isHidden(const boost::filesystem::path &p) const;

  bool showNewFolderModel;
  float panelWidth_FileOptions, panelWidth_FileOptionsMin;
  char fileName[256] = "untitled";
  char newFolderName[256] = "untitled";
  std::string currentFolder;
  int positionX, positionY, width, height;

  int Setting_Forward, Setting_Up;
};

#endif /* ExportOBJ_hpp */
