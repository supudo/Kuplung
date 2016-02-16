//
//  UI.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef UI_hpp
#define UI_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/implementation/SDL2OpenGL32.hpp"
#include "kuplung/ui/components/Log.hpp"
#include "kuplung/ui/components/Screenshot.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"
#include "kuplung/ui/components/Editor.hpp"
#include "kuplung/ui/dialogs/DialogStyle.hpp"
#include "kuplung/ui/dialogs/DialogOptions.hpp"
#include "kuplung/ui/dialogs/DialogControlsGUI.hpp"
#include "kuplung/ui/dialogs/DialogControlsModels.hpp"
#include "kuplung/meshes/ModelFace.hpp"

class UI {
public:
    ~UI();
    void destroy();
    void init(SDL_Window *window,
              ObjectsManager *managerObjects,
              std::function<void()> quitApp,
              std::function<void(FBEntity)> processFile,
              std::function<void()> newScene,
              std::function<void(std::string)> fileShaderCompile);

    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame);
    void renderEnd();
    void doLog(std::string message);

    void recentFilesAdd(std::string title, FBEntity file);
    void recentFilesClear();
    bool isMouseOnGUI();
    void showLoading();
    void hideLoading();

    std::vector<ModelFace*> * meshModelFaces;
    bool isFrame;
    bool isLoadingOpen;
    bool showControlsGUI;
    bool showControlsModels;
    float loadingPercentage;

private:
    SDL_Window *sdlWindow;
    std::function<void()> funcQuitApp;
    std::function<void(FBEntity)> funcProcessFile;
    std::function<void()> funcNewScene;
    std::function<void(std::string)> funcFileShaderCompile;

    void dialogFileBrowserProcessFile(FBEntity file);
    void fileShaderEditorSaved(std::string fileName);

    void dialogFileBrowser();
    void dialogStyle();
    void dialogScreenshot();
    void dialogShaderEditor();
    void dialogLog();
    void dialogAppMetrics();
    void dialogAboutImGui();
    void dialogAboutKuplung();
    void dialogOptions(ImGuiStyle* ref = NULL);
    void dialogSceneStats();
    void dialogControlsGUI();
    void dialogControlsModels();

    SDL2OpenGL32 *imguiImplementation;
    ObjectsManager *managerObjects;
    Log *componentLog;
    Screenshot *componentScreenshot;
    FileBrowser *componentFileBrowser;
    Editor *componentFileEditor;
    DialogStyle *windowStyle;
    DialogOptions *windowOptions;
    DialogControlsGUI *controlsGUI;
    DialogControlsModels *controlsModels;

    std::map <std::string, FBEntity> recentFiles;
    bool needsFontChange;

    bool showDialogStyle;
    bool showDialogFile;
    bool showShaderEditor;
    bool showScreenshotWindow;
    bool showSceneStats;
    bool showOptions;
    bool showAppMetrics;
    bool showAboutImgui;
    bool showAboutKuplung;
    bool showDemoWindow;
};

#endif /* UI_hpp */
