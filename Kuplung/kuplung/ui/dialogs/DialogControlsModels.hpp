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
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/ui/UIHelpers.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/ui/components/materialeditor/MaterialEditor.hpp"
#include "kuplung/utilities/shapes/Shapes.h"
#include "kuplung/ui/components/uveditor/UVEditor.hpp"

class DialogControlsModels {
public:
  explicit DialogControlsModels(ObjectsManager &managerObjects);
  void init(SDL_Window* sdlWindow,
            const std::function<void(ShapeType)>& addShape,
            const std::function<void(LightSourceType)>& addLight,
            const std::function<void(int)>& deleteModel);
  void render(bool* show, bool* isFrame, std::vector<ModelFaceBase*> * mmfaces, int * sceneSelectedModelObject);

  int selectedObject;

private:
  std::function<void(ShapeType)> funcAddShape;
  std::function<void(LightSourceType)> funcAddLight;
  std::function<void(int)> funcDeleteModel;

  void drawModels(bool* isFrame, std::vector<ModelFaceBase*> * meshModelFaces);
  void drawCreate();

  void contextModelRename(std::vector<ModelFaceBase*> * meshModelFaces);
  void contextModelDelete();

  SDL_Window* sdlWindow;

  std::vector<ModelFaceBase*> * meshModelFaces;
  bool cmenu_deleteYn, cmenu_renameModel;
  char guiModelRenameText[256] = "";
  int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight, selectedTabPanel;
  float heightTopPanel = 170.0f;
  float panelHeight_Tabs = 36.0f;
  std::string TextureImage, TextureFilename;

  bool showTextureWindow_Ambient, showTexture_Ambient, showTextureWindow_Diffuse, showTexture_Diffuse;
  bool showTextureWindow_Dissolve, showTexture_Dissolve, showTextureWindow_Bump, showTexture_Bump;
  bool showTextureWindow_Displacement, showTexture_Displacement;
  bool showTextureWindow_Specular, showTexture_Specular, showTextureWindow_SpecularExp, showTexture_SpecularExp;
  bool showUVEditor;

  int textureAmbient_Width, textureAmbient_Height, textureDiffuse_Width, textureDiffuse_Height;
  int textureDissolve_Width, textureDissolve_Height, textureBump_Width, textureBump_Height;
  int textureDisplacement_Width, textureDisplacement_Height;
  int textureSpecular_Width, textureSpecular_Height, textureSpecularExp_Width, textureSpecularExp_Height;

  GLuint vboTextureAmbient, vboTextureDiffuse, vboTextureDissolve, vboTextureBump;
  GLuint vboTextureDisplacement, vboTextureSpecular, vboTextureSpecularExp;

  void showTextureLine(std::string const& chkLabel, MaterialTextureType texType, bool* showWindow, bool* loadTexture);
  void createTextureBuffer(std::string imageFile, GLuint* vboBuffer, int* width, int* height) const;
  void showTextureImage(ModelFaceBase* mmf, MaterialTextureType type, std::string title, bool* showWindow, bool* genTexture, GLuint* vboBuffer, int* width, int* height) const;
  void processTexture(ModelFaceBase *mmf);

  ObjectsManager &managerObjects;
  std::unique_ptr<UIHelpers> helperUI;
  std::unique_ptr<MaterialEditor> componentMaterialEditor;
  std::unique_ptr<UVEditor> componentUVEditor;
};

#endif /* DialogControlsModels_hpp */
