//
//  RendererUI.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef RendererUI_hpp
#define RendererUI_hpp

#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/ui/components/FileSaver.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/utilities/renderers/ImageRenderer.hpp"

class RendererUI {
public:
  void init(SDL_Window* sdlWindow);
  void render(bool* show, ImageRenderer* imageRenderer, ObjectsManager* managerObjects, std::vector<ModelFaceBase*>* meshModelFaces);

  int wWidth, wHeight, wPadding;
  float bHeight;

private:
  void renderImageTexture(ImageRenderer* imageRenderer, std::vector<ModelFaceBase*>* meshModelFaces);

  std::unique_ptr<FileSaver> componentFileSaver;
  void dialogFileSaveProcessFile(const FBEntity& file, FileSaverOperation operation);
  bool showSaveDialog;
  void dialogFileSave();

  int rendererType, imageFormat;
  float panelWidth_RenderOptions, panelWidth_RenderOptionsMin, zoomFactor;
  ImVec2 scrolling = ImVec2(0.0f, 0.0f);

  bool genTexture;
  GLuint vboBuffer;
  int tWidth, tHeight;
  std::string currentFileImage;
  void createTextureBuffer();
};

#endif /* RendererUI_hpp */
