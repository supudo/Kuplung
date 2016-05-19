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
#include "kuplung/ui/components/uveditor/UVPoint.hpp"

struct UVLine {
    ImVec2 positionX, positionY;
    ImColor color;
};

class UVEditor {
public:
    void init(int positionX, int positionY, int width, int height);
    void setModel(ModelFace *mmf, MaterialTextureType texType, std::string texturePath, std::function<void(ModelFace*)> funcProcessTexture);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::function<void(ModelFace*)> funcProcessTexture;

    ModelFace *mmf;
    std::string texturePath, textureImage, textureFilename;
    int positionX, positionY, width, height, textureWidth, textureHeight;
    bool showFileBrowser, textureLoaded;
    char filePath[256];
    MaterialTextureType textureType;
    GLuint vboTexture;
    ImVec2 scrolling = ImVec2(0.0f, 0.0f);
    int uvUnwrappingType, uvUnwrappingTypePrev;
    ImColor pColor = ImColor(255, 24, 0);
    ImColor lColor = ImColor(255, 112, 0);
    ImColor overlayColor = ImColor(255, 112, 0, 100);
    float pRadius = 5.0f;

    std::vector<UVPoint *> uvPoints;
    std::vector<UVLine> uvLines;

    FileBrowser *componentFileBrowser;

    void processTextureCoordinates();
    void projectSquare();
    void dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType);
};

#endif /* UVEditor_hpp */
