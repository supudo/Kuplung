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

#include "kuplung/utilities/imgui/imgui.h"

#include "kuplung/ui/components/Log.hpp"
#include "kuplung/ui/components/Screenshot.hpp"
#include "kuplung/ui/components/FileBrowser.hpp"
#include "kuplung/ui/components/importers/ImportFile.hpp"
#include "kuplung/ui/components/exporters/ExportFile.hpp"
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

class UIManager {
public:
    explicit UIManager(ObjectsManager &managerObjects);
    ~UIManager();
    void init(SDL_Window *window,
              SDL_GLContext glContext,
              const std::function<void()>& quitApp,
              const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat, int importFormatAssimp)>& processImportedFile,
              const std::function<void()>& newScene,
              const std::function<void(std::string)>& fileShaderCompile,
              const std::function<void(ShapeType)>& addShape,
              const std::function<void(LightSourceType)>& addLight,
              const std::function<void(FBEntity file, std::vector<std::string>, ImportExportFormats exportFormat, int exportFormatAssimp)>& exportScene,
              const std::function<void(int)>& deleteModel,
              const std::function<void(FBEntity file)>& renderScene,
              const std::function<void(FBEntity file)>& saveScene,
              const std::function<void(FBEntity file)>& openScene
              );

    void doLog(const std::string& message);
    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame, int * sceneSelectedModelObject);
    void renderEnd();
    void renderPostEnd() const;
    bool isMouseOnGUI() const;
    void showParsing();
    void hideParsing();
    void showLoading();
    void hideLoading();
    void showExporting();
    void hideExporting();
    void showRenderedImage(std::string const& renderedImage);
    void clearAllLights();
    void setSceneSelectedModelObject(int sceneSelectedModelObject);

    void renderComponentSVS();
    void renderComponentRenderer(ImageRenderer* comp);

    void setShowControlsGUI(bool showControls);
    void setShowControlsModels(bool showModels);
    void recentFilesAdd(const FBEntity& file);
    void recentFilesClear();
    void recentFilesAddImported(const FBEntity& file);
    void recentFilesClearImported();

    void setParcingPercentage(float value);

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
    std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat, int importFormatAssimp)> funcProcessImportedFile;
    std::function<void()> funcNewScene;
    std::function<void(std::string)> funcFileShaderCompile;
    std::function<void(ShapeType)> funcAddShape;
    std::function<void(LightSourceType)> funcAddLight;
    std::function<void(FBEntity file, std::vector<std::string>, ImportExportFormats exportFormat, int exportFormatAssimp)> funcProcessExportedFile;
    std::function<void(int)> funcDeleteModel;
    std::function<void(FBEntity file)> funcRenderScene;
    std::function<void(FBEntity file)> funcSaveScene;
    std::function<void(FBEntity file)> funcOpenScene;

    void dialogFileBrowserProcessFile(FBEntity const& file);
    void dialogImporterProcessFile(FBEntity const& file, std::vector<std::string> settings, ImportExportFormats importFormat, int importFormatAssimp);
    void dialogExporterProcessFile(FBEntity const& file, std::vector<std::string> settings, ImportExportFormats exportFormat, int exportFormatAssimp);
    void dialogFileSaveProcessFile(FBEntity const& file, FileSaverOperation type);
    void fileShaderEditorSaved(std::string const& fileName);

    ImportExportFormats dialogImportType, dialogExportType;
    int dialogImportType_Assimp, dialogExportType_Assimp;
    void dialogImporterBrowser();
    void dialogExporterBrowser();
    void dialogStyle();
    void dialogScreenshot();
    void dialogShaderEditor();
    void dialogLog();
    void dialogAppMetrics();
    void dialogAboutImGui();
    void dialogAboutKuplung();
    void dialogOptions(ImGuiStyle* ref = nullptr);
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
    std::unique_ptr<Log> componentLog;
    std::unique_ptr<Screenshot> componentScreenshot;
    std::unique_ptr<FileBrowser> componentFileBrowser;
    std::unique_ptr<ImportFile> componentImportFile;
    std::unique_ptr<ExportFile> componentExportFile;
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
    bool showImporterFile;
    bool showExporterFile;
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

#endif /* UIManager_hpp */
