//
//  ExportFile.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 17/08/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#ifndef ExportFile_hpp
#define ExportFile_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include <filesystem>

class ExportFile {
public:
  void init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats exportFormat, int exportFormatAssimp)>& saveFile);
  void draw(ImportExportFormats* dialogExportType, int* dialogExportType_Assimp, bool* p_opened = nullptr);

private:
  std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
  std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats exportFormat, int exportFormatAssimp)> funcFileSave;

  void drawFiles(const std::string& fPath);
  const std::string convertToString(double num) const;
  const std::string convertSize(size_t size) const;
  const double roundOff(double n) const;
  void modalNewFolder();

  bool showNewFolderModel;
  float panelWidth_FileOptions, panelWidth_FileOptionsMin;
  char fileName[256] = "untitled";
  char newFolderName[256] = "untitled";
  std::string currentFolder;
  int positionX, positionY, width, height;

  int Setting_Forward, Setting_Up;
	std::vector<const char*> assimpExporters;
};

#endif /* ExportFile_hpp */
