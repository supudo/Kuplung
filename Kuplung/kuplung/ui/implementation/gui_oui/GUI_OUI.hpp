//
//  GUI_OUI.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUI_OUI_hpp
#define GUI_OUI_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"

#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/utilities/shapes/Shapes.h"

#include "kuplung/ui/implementation/gui_oui/OuiNanoVG_Implementation.hpp"

class GUI_OUI {
public:
    explicit GUI_OUI(ObjectsManager &managerObjects);
    ~GUI_OUI();
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

    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    void doLog(std::string const& message);
    void setSceneSelectedModelObject(int sceneSelectedModelObject);

    void recentFilesAdd(const FBEntity& file);
    void recentFilesClear();
    void recentFilesAddImported(const FBEntity& file);
    void recentFilesClearImported();
    bool isMouseOnGUI();
    void showParsing();
    void hideParsing();
    void showLoading();
    void hideLoading();
    void showExporting();
    void hideExporting();
    void showRenderedImage(std::string const& renderedImage);
    void clearAllLights();

    std::vector<ModelFaceBase*> *meshModelFaces;
    bool isFrame;
    bool isParsingOpen, isLoadingOpen, isExportingOpen;
    bool showControlsGUI;
    bool showControlsModels;
    float parsingPercentage;
    std::vector<FBEntity> recentFiles, recentFilesImported;

    bool showSVS;
    bool showRendererUI;

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
    std::unique_ptr<OuiNanoVG_Implementation> guiImplementation;
};

#endif /* GUI_OUI_hpp */
