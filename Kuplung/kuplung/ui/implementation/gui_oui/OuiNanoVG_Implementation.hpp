//
//  OuiNanoVG_Implementation.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef OuiNanoVG_Implementation_hpp
#define OuiNanoVG_Implementation_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"

#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/shapes/Shapes.h"

class OuiNanoVG_Implementation {
public:
    explicit OuiNanoVG_Implementation(ObjectsManager &managerObjects);
    ~OuiNanoVG_Implementation();
    void init(SDL_Window *window,
              std::function<void()> quitApp,
              std::function<void(FBEntity, std::vector<std::string>)> processImportedFile,
              std::function<void()> newScene,
              std::function<void(std::string)> fileShaderCompile,
              std::function<void(ShapeType)> addShape,
              std::function<void(LightSourceType)> addLight,
              std::function<void(FBEntity file, std::vector<std::string>)> exportScene,
              std::function<void(int)> deleteModel,
              std::function<void(FBEntity file)> renderScene,
              std::function<void(FBEntity file)> saveScene,
              std::function<void(FBEntity file)> openScene
              );

    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();

private:
    SDL_Window *sdlWindow;
    std::function<void()> funcQuitApp;
    std::function<void(FBEntity, std::vector<std::string>)> funcProcessImportedFile;
    std::function<void()> funcNewScene;
    std::function<void(std::string)> funcFileShaderCompile;
    std::function<void(ShapeType)> funcAddShape;
    std::function<void(LightSourceType)> funcAddLight;
    std::function<void(FBEntity file, std::vector<std::string>)> funcProcessExpoterdFile;
    std::function<void(int)> funcDeleteModel;
    std::function<void(FBEntity file)> funcRenderScene;
    std::function<void(FBEntity file)> funcSaveScene;
    std::function<void(FBEntity file)> funcOpenScene;

    ObjectsManager &managerObjects;
};

#endif /* OuiNanoVG_Implementation_hpp */
