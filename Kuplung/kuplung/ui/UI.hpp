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
#include "kuplung/ui/components/FileSaver.hpp"
#include "kuplung/ui/components/ShaderEditor.hpp"
#include "kuplung/ui/dialogs/DialogStyle.hpp"
#include "kuplung/ui/dialogs/DialogOptions.hpp"
#include "kuplung/ui/dialogs/DialogControlsGUI.hpp"
#include "kuplung/ui/dialogs/DialogControlsModels.hpp"
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/utilities/shapes/Shapes.h"

class UI {
public:
    ~UI();
    void destroy();
    void init(SDL_Window *window,
              ObjectsManager *managerObjects,
              std::function<void()> quitApp,
              std::function<void(FBEntity, FileBrowser_ParserType)> processFile,
              std::function<void()> newScene,
              std::function<void(std::string)> fileShaderCompile,
              std::function<void(ShapeType)> addShape,
              std::function<void(LightSourceType)> addLight,
              std::function<void(FBEntity file)> exportScene,
              std::function<void(int)> deleteModel,
              std::function<void(FBEntity file)> renderScene
              );

    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    void doLog(std::string message);
    void setSceneSelectedModelObject(int sceneSelectedModelObject);

    void recentFilesAdd(std::string title, FBEntity file);
    void recentFilesClear();
    bool isMouseOnGUI();
    void showParsing();
    void hideParsing();
    void showLoading();
    void hideLoading();
    void showExporting();
    void hideExporting();

    std::vector<ModelFace*> *meshModelFaces;
    bool isFrame;
    bool isParsingOpen, isLoadingOpen, isExportingOpen;
    bool showControlsGUI;
    bool showControlsModels;
    float parsingPercentage;
    std::map <std::string, FBEntity> recentFiles;

private:
    SDL_Window *sdlWindow;
    std::function<void()> funcQuitApp;
    std::function<void(FBEntity, FileBrowser_ParserType)> funcProcessFile;
    std::function<void()> funcNewScene;
    std::function<void(std::string)> funcFileShaderCompile;
    std::function<void(ShapeType)> funcAddShape;
    std::function<void(LightSourceType)> funcAddLight;
    std::function<void(FBEntity file)> funcExportScene;
    std::function<void(int)> funcDeleteModel;
    std::function<void(FBEntity file)> funcRenderScene;

    void dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType type);
    void dialogFileSaveProcessFile(FBEntity file, FileSaverOperation type);
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
    void dialogControlsModels(int * sceneSelectedModelObject);
    void dialogFileSave(FileSaverOperation type);

    SDL2OpenGL32 *imguiImplementation;
    ObjectsManager *managerObjects;
    Log *componentLog;
    Screenshot *componentScreenshot;
    FileBrowser *componentFileBrowser;
    FileSaver *componentFileSaver;
    ShaderEditor *componentFileEditor;
    DialogStyle *windowStyle;
    DialogOptions *windowOptions;
    DialogControlsGUI *controlsGUI;
    DialogControlsModels *controlsModels;

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
    bool showOBJExporter;
    bool showImageSave;
};

#endif /* UI_hpp */
