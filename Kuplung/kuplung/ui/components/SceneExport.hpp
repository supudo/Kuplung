//
//  SceneExport.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SceneExport_hpp
#define SceneExport_hpp

#include "kuplung/utilities/imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include "kuplung/settings/Settings.h"

class SceneExport {
public:
    void init(int positionX, int positionY, int width, int height, std::function<void(FBEntity)> exportFile);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::map<std::string, FBEntity> getFolderContents(std::string filePath);
    std::function<void(FBEntity)> funcExportFile;

    void drawFiles();
    std::string convertToString(double num);
    std::string convertSize(size_t size);
    double roundOff(double n);
    void modalNewFolder();

    bool showNewFolderModel;
    float panelWidth_FileOptions;
    char fileName[256] = "untitled";
    char newFolderName[256] = "untitled";
    std::string currentFolder;
    int positionX, positionY, width, height;
};

#endif /* SceneExport_hpp */
