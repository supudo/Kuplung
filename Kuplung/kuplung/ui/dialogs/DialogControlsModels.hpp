//
//  DialogControlsModels.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogControlsModels_hpp
#define DialogControlsModels_hpp

#include <regex>
#include <string>
#include <vector>
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/ui/UIHelpers.hpp"
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditor.hpp"
#include "kuplung/utilities/shapes/Shapes.h"
#include "kuplung/ui/components/FileBrowser.hpp"

class DialogControlsModels {
public:
    void init(SDL_Window* sdlWindow, ObjectsManager *managerObjects, std::function<void(ShapeType)> addShape, std::function<void(LightSourceType)> addLight);
    void render(bool* show, bool* isFrame, std::vector<ModelFace*> * meshModelFaces, std::vector<objScene> *scenes, int * sceneSelectedModelObject);

    int selectedObject;

private:
    std::function<void(ShapeType)> funcAddShape;
    std::function<void(LightSourceType)> funcAddLight;

    void drawModels(bool* isFrame, std::vector<ModelFace*> * meshModelFaces, std::vector<objScene> *scenes);
    void drawCreate();

    void contextModelRename(std::vector<ModelFace*> * meshModelFaces);
    void contextModelDelete(std::vector<ModelFace*> * meshModelFaces);
    void dialogFileBrowserProcessFile(FBEntity file);

    SDL_Window* sdlWindow;

    std::vector<ModelFace*> * meshModelFaces;
    bool cmenu_deleteYn, cmenu_renameModel, showFileBrowser;
    char guiModelRenameText[256];
    int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight, selectedTabPanel;
    float heightTopPanel = 170.0f;
    float panelHeight_Tabs = 36.0f;
    char filePath[256];
    std::string TextureImage, TextureFilename;

    bool showTextureWindow_Ambient, showTexture_Ambient, showTextureWindow_Diffuse, showTexture_Diffuse;
    bool showTextureWindow_Dissolve, showTexture_Dissolve, showTextureWindow_Bump, showTexture_Bump;
    bool showTextureWindow_Displacement, showTexture_Displacement;
    bool showTextureWindow_Specular, showTexture_Specular, showTextureWindow_SpecularExp, showTexture_SpecularExp;

    int textureAmbient_Width, textureAmbient_Height, textureDiffuse_Width, textureDiffuse_Height;
    int textureDissolve_Width, textureDissolve_Height, textureBump_Width, textureBump_Height;
    int textureDisplacement_Width, textureDisplacement_Height;
    int textureSpecular_Width, textureSpecular_Height, textureSpecularExp_Width, textureSpecularExp_Height;

    GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureDissolve, vboTextureBump;
    GLuint vboTextureDisplacement, vboTextureSpecular, vboTextureSpecularExp;

    void showTextureLine(std::string chkLabel, std::string title, bool* useTexture, bool* showWindow, bool* loadTexture, std::string image);
    void createTextureBuffer(std::string imageFile, GLuint* vboBuffer, int* width, int* height);
    void showTextureImage(ModelFace* mmf, int type, std::string title, bool* showWindow, bool* genTexture, GLuint* vboBuffer, int* width, int* height);
    void showTextureAdd(MaterialTextureType mtType);

    ObjectsManager *managerObjects;
    UIHelpers *helperUI;
    MaterialEditor *componentMaterialEditor;
    FileBrowser *componentFileBrowser;
};

#endif /* DialogControlsModels_hpp */
