//
//  GUI_ImGui.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUI_ImGui_hpp
#define GUI_ImGui_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLIncludes.h"

#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/ui/implementation/gui_imgui/SDL2OpenGL32.hpp"

#include "kuplung/ui/components/Log.hpp"
#include "kuplung/ui/components/Screenshot.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"
#include "kuplung/ui/components/importers/ImportFile.hpp"
#include "kuplung/ui/components/exporters/ExportOBJ.hpp"
#include "kuplung/ui/components/FileSaver.hpp"
#include "kuplung/ui/components/ImageViewer.hpp"
#include "kuplung/ui/components/RendererUI.hpp"
#include "kuplung/ui/components/ShaderEditor.hpp"
#include "kuplung/ui/components/ide/KuplungIDE.hpp"

#ifdef DEF_KuplungSetting_UseCuda
#include "kuplung/cuda/CudaExamples.hpp"
#endif

#include "kuplung/ui/dialogs/DialogStyle.hpp"
#include "kuplung/ui/dialogs/DialogOptions.hpp"
#include "kuplung/ui/dialogs/DialogControlsGUI.hpp"
#include "kuplung/ui/dialogs/DialogControlsModels.hpp"
#include "kuplung/ui/dialogs/DialogSVS.hpp"
#include "kuplung/ui/dialogs/DialogShadertoy.hpp"

#include "kuplung/meshes/scene/ModelFaceBase.hpp"

#include "kuplung/utilities/shapes/Shapes.h"
#include "kuplung/utilities/consumption/Consumption.hpp"

class GUI_ImGui {
public:
    explicit GUI_ImGui(ObjectsManager &managerObjects);
    ~GUI_ImGui();
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

    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    void doLog(std::string const& message);
    void setSceneSelectedModelObject(int sceneSelectedModelObject);

    void recentFilesAdd(FBEntity file);
    void recentFilesClear();
    void recentFilesAddImported(FBEntity file);
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

    std::unique_ptr<DialogSVS> componentSVS;
    bool showSVS;

    std::unique_ptr<RendererUI> componentRendererUI;
    bool showRendererUI;

#ifdef DEF_KuplungSetting_UseCuda
    std::unique_ptr<CudaExamples> componentCudaExamples;
#endif

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

    void dialogFileBrowserProcessFile(FBEntity const& file, FileBrowser_ParserType type);
    void dialogImporterProcessFile(FBEntity const& file, std::vector<std::string> settings);
    void dialogOBJExporterProcessFile(FBEntity const& file, std::vector<std::string> settings);
    void dialogFileSaveProcessFile(FBEntity const& file, FileSaverOperation type);
    void fileShaderEditorSaved(std::string const& fileName);

    void dialogImporterBrowser(FileBrowser_ParserType type);
    void dialogOBJExporterBrowser();
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
    void dialogShadertoy();
    void dialogShadertoyMessage();
    void dialogShadertoyMessageWindow();
    void popupRecentFileDoesntExists();
    void popupRecentFileImportedDoesntExists();
    void dialogKuplungIDE();
    void dialogCudaExamples();

    ObjectsManager &managerObjects;
    std::unique_ptr<SDL2OpenGL32> imguiImplementation;
    std::unique_ptr<Log> componentLog;
    std::unique_ptr<Screenshot> componentScreenshot;
    std::unique_ptr<FileBrowser> componentFileBrowser;
    std::unique_ptr<ImportFile> componentImportFile;
    std::unique_ptr<ExportOBJ> componentExportOBJ;
    std::unique_ptr<FileSaver> componentFileSaver;
    std::unique_ptr<ShaderEditor> componentFileEditor;
    std::unique_ptr<DialogStyle> windowStyle;
    std::unique_ptr<DialogOptions> windowOptions;
    std::unique_ptr<DialogControlsGUI> controlsGUI;
    std::unique_ptr<DialogControlsModels> controlsModels;
    std::unique_ptr<DialogShadertoy> componentShadertoy;
    std::unique_ptr<KuplungApp::Utilities::Consumption::Consumption> componentConsumption;
    std::unique_ptr<ImageViewer> componentImageViewer;
    std::unique_ptr<KuplungIDE> componentKuplungIDE;

    bool needsFontChange;

    bool showDialogStyle;
    bool showDialogFile;
    bool showFileImporter_OBJ;
    bool showFileImporter_STL;
    bool showFileImporter_PLY;
    bool showOBJExporter;
    bool showSaveDialog;
    bool showOpenDialog;
    bool showShaderEditor;
    bool showScreenshotWindow;
    bool showSceneStats;
    bool showOptions;
    bool showAppMetrics;
    bool showAboutImgui;
    bool showAboutKuplung;
    bool showDemoWindow;
    bool showImageSave;
    bool showRenderer;
    bool showRecentFileDoesntExists;
    bool showRecentFileImportedDoesntExists;
    bool showShadertoy;
    bool showShadertoyMessage;
    bool showImageViewer;
    bool showKuplungIDE;
    bool showCudaExamples;

};

#endif /* GUI_ImGui_hpp */
