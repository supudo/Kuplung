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
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/ui/components/uveditor/UVPoint.hpp"

struct UVLine {
    ImVec2 positionX, positionY;
    ImColor color;
};

typedef enum UVUnwrappingMethod {
    UVUnwrappingMethod_None,
    UVUnwrappingMethod_Default,
    UVUnwrappingMethod_Cube,
    UVUnwrappingMethod_Cylinder,
    UVUnwrappingMethod_Sphere
} UVUnwrappingMethod;

class UVEditor {
public:
    void init(int positionX, int positionY, int width, int height);
    void setModel(ModelFaceBase *mmf, MaterialTextureType texType, std::string const& texturePath, std::function<void(ModelFaceBase*)> funcProcessTexture);
    void draw(const char* title, bool* p_opened = NULL);

private:
    std::function<void(ModelFaceBase*)> funcProcessTexture;

    ModelFaceBase *mmf;
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

    void projectSquare();
    void initTextureBuffer();
    void dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType);
    void processTextureCoordinates();
    void processTextureCoordinatesSquare();
};

#endif /* UVEditor_hpp */
