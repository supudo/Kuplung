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

#include "kuplung/utilities/nanovg/nanovg.h"

class OuiNanoVG_Implementation {
public:
    explicit OuiNanoVG_Implementation(ObjectsManager &managerObjects);
    ~OuiNanoVG_Implementation();
    void init(SDL_Window *window,
			  const std::function<void()>& quitApp,
			  const std::function<void(FBEntity, std::vector<std::string>)>& processImportedFile,
			  const std::function<void()>& newScene,
			  const std::function<void(std::string)>& fileShaderCompile,
			  const std::function<void(ShapeType)>& addShape,
			  const std::function<void(LightSourceType)>& addLight,
			  const std::function<void(FBEntity file, std::vector<std::string>, ImportExportFormats exportFormat)>& exportScene,
			  const std::function<void(int)>& deleteModel,
			  const std::function<void(FBEntity file)>& renderScene,
			  const std::function<void(FBEntity file)>& saveScene,
			  const std::function<void(FBEntity file)>& openScene
              );

    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    bool processEvent(SDL_Event *event);

private:
    SDL_Window *sdlWindow;
    std::function<void()> funcQuitApp;
    std::function<void(FBEntity, std::vector<std::string>)> funcProcessImportedFile;
    std::function<void()> funcNewScene;
    std::function<void(std::string)> funcFileShaderCompile;
    std::function<void(ShapeType)> funcAddShape;
    std::function<void(LightSourceType)> funcAddLight;
    std::function<void(FBEntity file, std::vector<std::string>, ImportExportFormats exportFormat)> funcProcessExportedFile;
    std::function<void(int)> funcDeleteModel;
    std::function<void(FBEntity file)> funcRenderScene;
    std::function<void(FBEntity file)> funcSaveScene;
    std::function<void(FBEntity file)> funcOpenScene;

    ObjectsManager &managerObjects;
};

#endif /* OuiNanoVG_Implementation_hpp */
