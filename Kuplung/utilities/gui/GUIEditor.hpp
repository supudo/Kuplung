//
//  GUIEditor.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUIEditor_hpp
#define GUIEditor_hpp

#include "imgui/imgui.h"
#include <functional>
#include <map>
#include <string>
#include "utilities/settings/Settings.h"

class GUIEditor {
public:
    void init(std::string appPath, int positionX, int positionY, int width, int height, std::function<void(std::string)> doLog);
    void draw(std::function<void(std::string, std::string)> fileShaderCompile, const char* title, bool* p_opened = NULL);
    
private:
    std::function<void(std::string)> doLog;
    std::function<void(std::string, std::string)> doFileShaderCompile;
    void logMessage(std::string logMessage);
    void compileShader();

    std::string appPath, fileContents, currentFileName;
    int positionX, positionY, width, height, shaderFileIndex;
    char guiEditorText[1024 * 16];
};

#endif /* GUIEditor_hpp */
