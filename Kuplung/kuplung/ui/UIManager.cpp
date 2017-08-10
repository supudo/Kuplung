//
//  UIManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "UIManager.hpp"

UIManager::UIManager(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->provider_ImGui = std::make_unique<GUI_ImGui>(managerObjects);
    this->provider_OUI = std::make_unique<GUI_OUI>(managerObjects);
}

UIManager::~UIManager() {
    this->provider_ImGui.reset();
}

void UIManager::init(SDL_Window *window,
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
                     ) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->init(window, quitApp, processImportedFile, newScene, fileShaderCompile,
                                              addShape, addLight, exportScene, deleteModel, renderScene, saveScene, openScene);
        case 2:
            return this->provider_OUI->init(window, quitApp, processImportedFile, newScene, fileShaderCompile,
                                              addShape, addLight, exportScene, deleteModel, renderScene, saveScene, openScene);
    }
}

void UIManager::doLog(const std::string& message) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->doLog(message);
            break;
        case 2:
            this->provider_OUI->doLog(message);
            break;
    }
}

void UIManager::setMeshModelFaces(std::vector<ModelFaceBase*> *meshModelFaces) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->meshModelFaces = meshModelFaces;
            break;
        case 2:
            this->provider_OUI->meshModelFaces = meshModelFaces;
            break;
    }
}

bool UIManager::processEvent(SDL_Event *event) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->processEvent(event);
        case 2:
            return this->provider_OUI->processEvent(event);
        default:
            return false;
    }
}

void UIManager::renderStart(bool isFrame, int * sceneSelectedModelObject) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->renderStart(isFrame, sceneSelectedModelObject);
            break;
        case 2:
            this->provider_OUI->renderStart(isFrame, sceneSelectedModelObject);
            break;
    }
}

void UIManager::renderEnd() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->renderEnd();
            break;
        case 2:
            this->provider_OUI->renderEnd();
            break;
    }
}

bool UIManager::isMouseOnGUI() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isMouseOnGUI();
        case 2:
            return this->provider_OUI->isMouseOnGUI();
        default:
            return false;
    }
}

void UIManager::showParsing() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showParsing();
            break;
        case 2:
            this->provider_OUI->showParsing();
            break;
    }
}

void UIManager::hideParsing() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideParsing();
            break;
        case 2:
            this->provider_OUI->hideParsing();
            break;
    }
}

void UIManager::showLoading() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showLoading();
            break;
        case 2:
            this->provider_OUI->showLoading();
            break;
    }
}

void UIManager::hideLoading() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideLoading();
            break;
        case 2:
            this->provider_OUI->hideLoading();
            break;
    }
}

void UIManager::showExporting() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showExporting();
            break;
        case 2:
            this->provider_OUI->showExporting();
            break;
    }
}

void UIManager::hideExporting() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->hideExporting();
            break;
        case 2:
            this->provider_OUI->hideExporting();
            break;
    }
}

void UIManager::showRenderedImage(std::string const& renderedImage) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showRenderedImage(renderedImage);
            break;
        case 2:
            this->provider_OUI->showRenderedImage(renderedImage);
            break;
    }
}

void UIManager::clearAllLights() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->clearAllLights();
            break;
        case 2:
            this->provider_OUI->clearAllLights();
            break;
    }
}

bool UIManager::isParsingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isParsingOpen;
        case 2:
            return this->provider_OUI->isParsingOpen;
        default:
            return false;
    }
}

bool UIManager::isLoadingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isLoadingOpen;
        case 2:
            return this->provider_OUI->isLoadingOpen;
        default:
            return false;
    }
}

bool UIManager::isExportingOpen() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->isExportingOpen;
        case 2:
            return this->provider_OUI->isExportingOpen;
        default:
            return false;
    }
}

void UIManager::setSceneSelectedModelObject(int sceneSelectedModelObject) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->setSceneSelectedModelObject(sceneSelectedModelObject);
            break;
        case 2:
            this->provider_OUI->setSceneSelectedModelObject(sceneSelectedModelObject);
            break;
    }
}

bool UIManager::showSVS() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->showSVS;
        case 2:
            return this->provider_OUI->showSVS;
        default:
            return false;
    }
}

void UIManager::renderComponentSVS() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->componentSVS->render(&this->provider_ImGui->showSVS);
            break;
        case 2:
//            this->provider_OUI->componentSVS->render(&this->provider_ImGui->showSVS);
            break;
    }
}

bool UIManager::showRendererUI() {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            return this->provider_ImGui->showRendererUI;
        case 2:
            return this->provider_OUI->showRendererUI;
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
        case 2:
//            this->provider_OUI->componentRendererUI->render(&this->provider_ImGui->showRendererUI,
//                                                              comp,
//                                                              &this->managerObjects,
//                                                              this->provider_ImGui->meshModelFaces);
            break;
    }
}

void UIManager::setShowControlsGUI(bool showControls) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showControlsGUI = showControls;
            break;
        case 2:
            this->provider_OUI->showControlsGUI = showControls;
            break;
    }
}

void UIManager::setShowControlsModels(bool showModels) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->showControlsModels = showModels;
            break;
        case 2:
            this->provider_OUI->showControlsModels = showModels;
            break;
    }
}

void UIManager::setRecentFiles(const std::vector<FBEntity>& files) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFiles = files;
            break;
        case 2:
            this->provider_OUI->recentFiles = files;
            break;
    }
}

void UIManager::setRecentFilesImported(const std::vector<FBEntity>& files) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesImported = files;
            break;
        case 2:
            this->provider_OUI->recentFilesImported = files;
            break;
    }
}

void UIManager::recentFilesAdd(const FBEntity& file) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesAdd(file);
            break;
        case 2:
            this->provider_OUI->recentFilesAdd(file);
            break;
    }
}

void UIManager::recentFilesAddImported(const FBEntity& file) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->recentFilesAddImported(file);
            break;
        case 2:
            this->provider_OUI->recentFilesAddImported(file);
            break;
    }
}

void UIManager::setParcingPercentage(float value) {
    switch (Settings::Instance()->GUISystem) {
        case 1:
            this->provider_ImGui->parsingPercentage = value;
            break;
        case 2:
            this->provider_OUI->parsingPercentage = value;
            break;
    }
}
