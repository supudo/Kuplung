//
//  UIManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "UIManager.hpp"

UIManager::UIManager(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
    this->provider_ImGui = std::make_unique<GUI_ImGui>(managerObjects);
}

UIManager::~UIManager() {
    this->provider_ImGui.reset();
}

void UIManager::init(SDL_Window *window,
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
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->init(window, quitApp, processImportedFile, newScene, fileShaderCompile,
                                              addShape, addLight, exportScene, deleteModel, renderScene, saveScene, openScene);
            break;
    }
}

void UIManager::doLog(std::string message) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->doLog(message);
    }
}

void UIManager::setMeshModelFaces(std::vector<ModelFaceBase*> *meshModelFaces) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->meshModelFaces = meshModelFaces;
            break;
    }
}

bool UIManager::processEvent(SDL_Event *event) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->processEvent(event);
        default:
            return false;
    }
}

void UIManager::renderStart(bool isFrame, int * sceneSelectedModelObject) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->renderStart(isFrame, sceneSelectedModelObject);
            break;
    }
}

void UIManager::renderEnd() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->renderEnd();
            break;
    }
}

bool UIManager::isMouseOnGUI() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isMouseOnGUI();
        default:
            return false;
    }
}

void UIManager::showParsing() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showParsing();
            break;
    }
}

void UIManager::hideParsing() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideParsing();
            break;
    }
}

void UIManager::showLoading() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showLoading();
            break;
    }
}

void UIManager::hideLoading() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideLoading();
            break;
    }
}

void UIManager::showExporting() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showExporting();
            break;
    }
}

void UIManager::hideExporting() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideExporting();
            break;
    }
}

void UIManager::showRenderedImage(std::string const& renderedImage) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showRenderedImage(renderedImage);
            break;
    }
}

void UIManager::clearAllLights() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->clearAllLights();
            break;
    }
}

bool UIManager::isParsingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isParsingOpen;
        default:
            return false;
    }
}

bool UIManager::isLoadingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isLoadingOpen;
        default:
            return false;
    }
}

bool UIManager::isExportingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isExportingOpen;
        default:
            return false;
    }
}

void UIManager::setSceneSelectedModelObject(int sceneSelectedModelObject) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->setSceneSelectedModelObject(sceneSelectedModelObject);
    }
}

bool UIManager::showSVS() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->showSVS;
        default:
            return false;
    }
}

void UIManager::renderComponentSVS() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->componentSVS->render(&this->provider_ImGui->showSVS);
            break;
    }
}

bool UIManager::showRendererUI() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->showRendererUI;
        default:
            return false;
    }
}

void UIManager::renderComponentRenderer(ImageRenderer* comp) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->componentRendererUI->render(&this->provider_ImGui->showRendererUI,
                                                              comp,
                                                              &this->managerObjects,
                                                              this->provider_ImGui->meshModelFaces);
            break;
    }
}

void UIManager::setShowControlsGUI(bool showControls) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showControlsGUI = showControls;
            break;
    }
}

void UIManager::setShowControlsModels(bool showModels) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showControlsModels = showModels;
            break;
    }
}

void UIManager::setRecentFiles(std::vector<FBEntity> files) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFiles = files;
            break;
    }
}

void UIManager::setRecentFilesImported(std::vector<FBEntity> files) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesImported = files;
            break;
    }
}

void UIManager::recentFilesAdd(FBEntity file) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesAdd(file);
            break;
    }
}

void UIManager::recentFilesAddImported(FBEntity file) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesAddImported(file);
            break;
    }
}

void UIManager::setParcingPercentage(float value) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->parsingPercentage = value;
            break;
    }
}
