//
//  UIManager.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef UIManager_hpp
#define UIManager_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/ui/implementation/gui_imgui/GUI_ImGui.hpp"
#include "kuplung/ui/implementation/gui_oui/GUI_OUI.hpp"

class UIManager {
public:
    explicit UIManager(ObjectsManager &managerObjects);
    ~UIManager();
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

    void doLog(const std::string& message);
    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    bool isMouseOnGUI();
    void showParsing();
    void hideParsing();
    void showLoading();
    void hideLoading();
    void showExporting();
    void hideExporting();
    void showRenderedImage(std::string const& renderedImage);
    void clearAllLights();
    bool isParsingOpen();
    bool isLoadingOpen();
    bool isExportingOpen();
    void setSceneSelectedModelObject(int sceneSelectedModelObject);

    void setMeshModelFaces(std::vector<ModelFaceBase*> *meshModelFaces);

    bool showSVS();
    void renderComponentSVS();
    bool showRendererUI();
    void renderComponentRenderer(ImageRenderer* comp);

    void setShowControlsGUI(bool showControls);
    void setShowControlsModels(bool showModels);
    void setRecentFiles(const std::vector<FBEntity>& files);
    void setRecentFilesImported(const std::vector<FBEntity>& files);
    void recentFilesAdd(const FBEntity& file);
    void recentFilesAddImported(const FBEntity& file);

    void setParcingPercentage(float value);

    std::unique_ptr<GUI_ImGui> provider_ImGui;

private:
    std::unique_ptr<GUI_OUI> provider_OUI;

    ObjectsManager &managerObjects;

};

#endif /* UIManager_hpp */
