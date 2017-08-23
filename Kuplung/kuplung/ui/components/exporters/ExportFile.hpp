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
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include <functional>
#include <map>
#include <string>
#include <boost/filesystem.hpp>

class ExportFile {
public:
    void init(int positionX, int positionY, int width, int height, const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats exportFormat)>& saveFile);
    void draw(const char* title, int* dialogImportType, bool* p_opened = NULL);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string const& filePath);
    std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats exportFormat)> funcFileSave;

    void drawFiles(const std::string& fPath);
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    void modalNewFolder();
    bool isHidden(const boost::filesystem::path &p);

    bool showNewFolderModel;
    float panelWidth_FileOptions, panelWidth_FileOptionsMin;
    char fileName[256] = "untitled";
    char newFolderName[256] = "untitled";
    std::string currentFolder;
    int positionX, positionY, width, height;

    int Setting_Forward, Setting_Up;
};

#endif /* ExportFile_hpp */
