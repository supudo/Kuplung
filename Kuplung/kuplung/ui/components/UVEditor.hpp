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

struct UVPoint {
    ImVec2 position;
    ImColor color;
    float radius;
};

struct UVLine {
    ImVec2 positionX, positionY;
    ImColor color;
};

struct PackedVertex2 {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    bool operator<(const PackedVertex2 that) const{
        return memcmp((void*)this, (void*)&that, sizeof(PackedVertex2))>0;
    };
};

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
    int uvUnwrappingType, uvUnwrappingTypePrev;

    std::vector<UVPoint> uvPoints;
    std::vector<UVLine> uvLines;

    FileBrowser *componentFileBrowser;

    void processTextureCoordinates();
    void projectSquare();
    bool getSimilarVertexIndex2(PackedVertex2 & packed, std::map<PackedVertex2, unsigned int> & vertexToOutIndex, unsigned int & result);
    void dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType);
};

#endif /* UVEditor_hpp */
