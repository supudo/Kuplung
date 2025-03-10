//
//  ImportFile.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImportFile_hpp
#define ImportFile_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <filesystem>

class ImportFile {
public:
  void init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat, int importFormatAssimp)>& processFile);
  void draw(ImportExportFormats* dialogImportType, int* dialogImportType_Assimp, bool* p_opened = nullptr);

private:
  std::map<std::string, FBEntity> getFolderContents(ImportExportFormats* dialogImportType, std::string const& filePath);
  std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat, int importFormatAssimp)> processFile;

  void drawFiles(ImportExportFormats* dialogImportType, int* dialogImportType_Assimp, const std::string& fPath);
  const std::string convertToString(double num) const;
  const std::string convertSize(size_t size) const;
  const double roundOff(double n) const;

  int positionX, positionY, width, height;
  float panelWidth_Options, panelWidth_OptionsMin;

  int Setting_Forward, Setting_Up;

	std::string currentFolder;
	std::vector<const char*> assimpImporters;
};

#endif /* ImportFile_hpp */
