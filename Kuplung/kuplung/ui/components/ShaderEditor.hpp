//
//  ShaderEditor.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ShaderEditor_hpp
#define ShaderEditor_hpp

#include <functional>
#include <map>
#include <string>
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/settings/Settings.h"

class ShaderEditor {
public:
    void init(std::string appPath, int positionX, int positionY, int width, int height);
    void draw(std::function<void(std::string)> fileShaderCompile, const char* title, bool* p_opened = NULL);

private:
    std::function<void(std::string)> doFileShaderCompile;
    void compileShader();

    std::string appPath, fileContents, currentFileName;
    int positionX, positionY, width, height, shaderFileIndex;
    char guiEditorText[1024 * 16];
};

#endif /* ShaderEditor_hpp */
