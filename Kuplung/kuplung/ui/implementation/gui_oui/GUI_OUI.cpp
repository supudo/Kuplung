//
//  GUI_OUI.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "GUI_OUI.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"

GUI_OUI::GUI_OUI(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
    this->isFrame = false;
    this->isParsingOpen = false;
    this->isLoadingOpen = false;
    this->isExportingOpen = false;
    this->showControlsGUI = false;
    this->showControlsModels = false;
    this->parsingPercentage = 0.0f;
}

GUI_OUI::~GUI_OUI() {
}

void GUI_OUI::init(SDL_Window *window,
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
              ) {
    this->sdlWindow = window;
    this->funcQuitApp = quitApp;
    this->funcProcessImportedFile = processImportedFile;
    this->funcNewScene = newScene;
    this->funcFileShaderCompile = fileShaderCompile;
    this->funcAddShape = addShape;
    this->funcAddLight = addLight;
    this->funcProcessExpoterdFile = exportScene;
    this->funcDeleteModel = deleteModel;
    this->funcRenderScene = renderScene;
    this->funcSaveScene = saveScene;
    this->funcOpenScene = openScene;

    this->isFrame = false;
    this->isLoadingOpen = false;
    this->isParsingOpen = false;
    this->isExportingOpen = false;
    this->parsingPercentage = 0.0f;

    this->recentFiles.clear();
    this->recentFilesImported.clear();
    this->showControlsGUI = false;
    this->showControlsModels = false;
}

void GUI_OUI::doLog(std::string const& message) {
    printf("%s\n", message.c_str());
}

void GUI_OUI::setSceneSelectedModelObject(int sceneSelectedModelObject) {
}

bool GUI_OUI::processEvent(SDL_Event *event) {
    return true;
}

void GUI_OUI::renderStart(bool isFrame, int * sceneSelectedModelObject) {
    this->isFrame = isFrame;
}

void GUI_OUI::clearAllLights() {
}

void GUI_OUI::showRenderedImage(std::string const& renderedImage) {
}

void GUI_OUI::renderEnd() {
}

void GUI_OUI::recentFilesAdd(FBEntity file) {
    bool exists = false;
    for (size_t i=0; i<this->recentFiles.size(); i++) {
        if (this->recentFiles[i].path == file.path) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        this->recentFiles.push_back(file);
        Settings::Instance()->saveRecentFiles(this->recentFiles);
    }
}

void GUI_OUI::recentFilesClear() {
    this->recentFiles.clear();
    Settings::Instance()->saveRecentFiles(this->recentFiles);
}

void GUI_OUI::recentFilesAddImported(FBEntity file) {
    bool exists = false;
    for (size_t i=0; i<this->recentFilesImported.size(); i++) {
        if (this->recentFilesImported[i].path == file.path) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        this->recentFilesImported.push_back(file);
        Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
    }
}

void GUI_OUI::recentFilesClearImported() {
    this->recentFilesImported.clear();
    Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
}

bool GUI_OUI::isMouseOnGUI() {
    return ImGui::IsMouseHoveringAnyWindow();
}

void GUI_OUI::showParsing() {
    this->isParsingOpen = true;
}

void GUI_OUI::hideParsing() {
    this->isParsingOpen = false;
}

void GUI_OUI::showLoading() {
    this->isLoadingOpen = true;
}

void GUI_OUI::hideLoading() {
    this->isLoadingOpen = false;
}

void GUI_OUI::showExporting() {
    this->isExportingOpen = true;
}

void GUI_OUI::hideExporting() {
    this->isExportingOpen = false;
}
