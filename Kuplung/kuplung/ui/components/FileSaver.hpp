//
//  FileSaver.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef FileSaver_hpp
#define FileSaver_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include "kuplung/settings/Settings.h"
#include <filesystem>

typedef enum FileSaverOperation {
  FileSaverOperation_SaveScene,
  FileSaverOperation_OpenScene,
  FileSaverOperation_Renderer
} FileSaverOperation;

class FileSaver {
public:
  void init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, FileSaverOperation)>& saveFile);
  void draw(const char* title, FileSaverOperation type, bool* p_opened = nullptr);

private:
  std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
  std::function<void(FBEntity, FileSaverOperation)> funcFileSave;

  void drawFiles(const std::string& fPath);
  const std::string convertToString(double num) const;
  const std::string convertSize(size_t size) const;
  const double roundOff(double n) const;
  void modalNewFolder();

  bool showNewFolderModel;
  float panelWidth_FileOptions;
  char fileName[256] = "untitled";
  char newFolderName[256] = "untitled";
  std::string currentFolder;
  int positionX, positionY, width, height;
};

#endif /* FileSaver_hpp */
