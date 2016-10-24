//
//  RendererUI.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef RendererUI_hpp
#define RendererUI_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/ImGui/imgui.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/renderers/ImageRenderer.hpp"
#include "kuplung/ui/components/FileSaver.hpp"

class RendererUI {
public:
    void init(SDL_Window *sdlWindow);
    void render(bool* show, ImageRenderer *imageRenderer, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> *meshModelFaces);

    int wWidth, wHeight, wPadding;
    float bHeight;

private:
    void renderImageTexture(ImageRenderer *imageRenderer, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> *meshModelFaces);

    std::unique_ptr<FileSaver> componentFileSaver;
    void dialogFileSaveProcessFile(FBEntity file, FileSaverOperation type);
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
