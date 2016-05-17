//
//  UVEditor.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef UVEditor_hpp
#define UVEditor_hpp

#include <functional>
#include <map>
#include <string>
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/ui/components/FileBrowser.hpp"
#include "kuplung/meshes/ModelFace.hpp"

class UVEditor {
public:
    void init(int positionX, int positionY, int width, int height);
    void setModel(ModelFace *mmf, MaterialTextureType texType, std::string texturePath, std::function<void(ModelFace*, MaterialTextureType, std::string, std::vector<glm::vec2>)> funcProcessTexture);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::function<void(ModelFace*, MaterialTextureType, std::string, std::vector<glm::vec2>)> funcProcessTexture;

    ModelFace *mmf;
    std::string texturePath, textureImage, textureFilename;
    int positionX, positionY, width, height, textureWidth, textureHeight;
    bool showFileBrowser, textureLoaded;
    char filePath[256];
    MaterialTextureType textureType;
    GLuint vboTexture;
    ImVec2 scrolling = ImVec2(0.0f, 0.0f);

    FileBrowser *componentFileBrowser;

    void dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType);
};

#endif /* UVEditor_hpp */
