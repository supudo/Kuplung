//
//  UIManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "UIManager.hpp"
#include "kuplung/ui/components/Tabs.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/utilities/imgui/imgui_impl_opengl3.h"
#include "kuplung/utilities/imgui/imgui_impl_sdl.h"
#include "kuplung/utilities/imgui/imguizmo/ImGuizmo.h"

UIManager::UIManager(ObjectsManager& managerObjects) : managerObjects(managerObjects) {
  this->isFrame = false;
  this->isParsingOpen = false;
  this->isLoadingOpen = false;
  this->isExportingOpen = false;
  this->showControlsGUI = false;
  this->showControlsModels = false;
  this->parsingPercentage = 0.0f;
  this->showSVS = false;
  this->showRendererUI = false;
  this->needsFontChange = false;
  this->showDialogStyle = false;
  this->showDialogFile = false;
  this->showImporterFile = false;
  this->showExporterFile = false;
  this->showSaveDialog = false;
  this->showOpenDialog = false;
  this->showShaderEditor = false;
  this->showScreenshotWindow = false;
  this->showSceneStats = false;
  this->showOptions = false;
  this->showAppMetrics = false;
  this->showAboutImgui = false;
  this->showAboutKuplung = false;
  this->showDemoWindow = false;
  this->showImageSave = false;
  this->showRenderer = false;
  this->showRecentFileDoesntExists = false;
  this->showRecentFileImportedDoesntExists = false;
  this->showShadertoy = false;
  this->showShadertoyMessage = false;
  this->showImageViewer = false;
  this->showKuplungIDE = false;
  this->showCudaExamples = false;
}

UIManager::~UIManager() {
  this->componentLog.reset();
  this->componentScreenshot.reset();
  this->componentFileBrowser.reset();
  this->componentFileSaver.reset();
  this->componentFileEditor.reset();
  this->windowStyle.reset();
  this->windowOptions.reset();
  this->controlsGUI.reset();
  this->controlsModels.reset();
  this->componentShadertoy.reset();
  this->componentConsumption.reset();
  this->componentSVS.reset();
  this->componentImageViewer.reset();
  this->componentRendererUI.reset();
  this->componentImportFile.reset();
  this->componentExportFile.reset();
  this->componentKuplungIDE.reset();
#ifdef DEF_KuplungSetting_UseCuda
  this->componentCudaExamples.reset();
#endif
}

void UIManager::init(SDL_Window* window, SDL_GLContext glContext, const std::function<void()>& quitApp,
                     const std::function<void(FBEntity, std::vector<std::string>, ImportExportFormats importFormat,
                     int importFormatAssimp)>& processImportedFile, const std::function<void()>& newScene,
                     const std::function<void(std::string)>& fileShaderCompile, const std::function<void(ShapeType)>& addShape,
                     const std::function<void(LightSourceType)>& addLight,
                     const std::function<void(FBEntity file, std::vector<std::string>, ImportExportFormats exportFormat,
                     int exportFormatAssimp)>& exportScene, const std::function<void(int)>& deleteModel,
                     const std::function<void(FBEntity file)>& renderScene, const std::function<void(FBEntity file)>& saveScene,
                     const std::function<void(FBEntity file)>& openScene) {
  this->sdlWindow = window;
  this->funcQuitApp = quitApp;
  this->funcProcessImportedFile = processImportedFile;
  this->funcNewScene = newScene;
  this->funcFileShaderCompile = fileShaderCompile;
  this->funcAddShape = addShape;
  this->funcAddLight = addLight;
  this->funcProcessExportedFile = exportScene;
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
  this->needsFontChange = false;
  this->showDialogStyle = false;
  this->showDialogFile = false;
  this->showControlsGUI = false;
  this->showControlsModels = false;
  this->showShaderEditor = false;
  this->showScreenshotWindow = false;
  this->showSceneStats = false;
  this->showOptions = false;
  this->showAppMetrics = false;
  this->showAboutImgui = false;
  this->showAboutKuplung = false;
  this->showDemoWindow = false;
  this->showExporterFile = false;
  this->showImageSave = false;
  this->showImporterFile = false;
  this->showSaveDialog = false;
  this->showOpenDialog = false;
  this->showRenderer = false;
  this->showRecentFileDoesntExists = false;
  this->showRecentFileImportedDoesntExists = false;
  this->showSVS = false;
  this->showShadertoy = false;
  this->showShadertoyMessage = false;
  this->showImageViewer = false;
  this->showRendererUI = false;
  this->showKuplungIDE = false;
  this->showCudaExamples = false;

  this->dialogImportType = ImportExportFormat_UNDEFINED;
  this->dialogExportType = ImportExportFormat_UNDEFINED;
  this->dialogImportType_Assimp = -1;
  this->dialogExportType_Assimp = -1;

  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
  int posX = 50, posY = 50;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  ImGui_ImplSDL2_InitForOpenGL(this->sdlWindow, glContext);
  ImGui_ImplOpenGL3_Init("#version 410 core");
  ImGui::StyleColorsDark();

  this->componentLog = std::make_unique<Log>();
  this->componentScreenshot = std::make_unique<Screenshot>();

  this->componentFileBrowser = std::make_unique<FileBrowser>();
  this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UIManager::dialogFileBrowserProcessFile, this, std::placeholders::_1));

  this->componentImportFile = std::make_unique<ImportFile>();
  this->componentImportFile->init(posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UIManager::dialogImporterProcessFile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  this->componentExportFile = std::make_unique<ExportFile>();
  this->componentExportFile->init(posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UIManager::dialogExporterProcessFile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  this->componentFileSaver = std::make_unique<FileSaver>();
  this->componentFileSaver->init(posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UIManager::dialogFileSaveProcessFile, this, std::placeholders::_1, std::placeholders::_2));

  this->componentFileEditor = std::make_unique<ShaderEditor>();
  this->componentFileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100);

  this->windowOptions = std::make_unique<DialogOptions>();
  this->windowOptions->init();

  this->windowStyle = std::make_unique<DialogStyle>();
  ImGuiStyle& style = ImGui::GetStyle();
  this->windowStyle->saveDefault(style);
  style = this->windowStyle->loadCurrent();

  this->windowOptions->loadFonts(&this->needsFontChange);

  this->controlsGUI = std::make_unique<DialogControlsGUI>(this->managerObjects);

  this->controlsModels = std::make_unique<DialogControlsModels>(this->managerObjects);
  this->controlsModels->init(this->sdlWindow, this->funcAddShape, this->funcAddLight, this->funcDeleteModel);

  this->componentSVS = std::make_unique<DialogSVS>();
  this->componentSVS->init();

  this->componentShadertoy = std::make_unique<DialogShadertoy>();
  this->componentShadertoy->init(std::bind(&UIManager::dialogShadertoyMessage, this));

  this->componentConsumption = std::make_unique<KuplungApp::Utilities::Consumption::Consumption>();
  this->componentConsumption->init();

  this->componentImageViewer = std::make_unique<ImageViewer>();

  this->componentRendererUI = std::make_unique<RendererUI>();
  this->componentRendererUI->init(this->sdlWindow);

  this->componentKuplungIDE = std::make_unique<KuplungIDE>();
  this->componentKuplungIDE->init();

#ifdef DEF_KuplungSetting_UseCuda
  this->componentCudaExamples = std::make_unique<CudaExamples>();
  this->componentCudaExamples->init();

  this->controlsGUI->setCudaOceanFFT(this->componentCudaExamples->exampleOceanFFT.get());
#endif
}

void UIManager::doLog(const std::string& message) {
  printf("%s\n", message.c_str());
  this->componentLog->addToLog("%s\n", message.c_str());
}

void UIManager::setSceneSelectedModelObject(int sceneSelectedModelObject) {
  this->controlsModels->selectedObject = sceneSelectedModelObject;
}

bool UIManager::processEvent(SDL_Event* event) {
  return ImGui_ImplSDL2_ProcessEvent(event);
}

void UIManager::renderStart(bool isFrame, int* sceneSelectedModelObject) {
  this->isFrame = isFrame;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(this->sdlWindow);
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem((char*)ICON_FA_FILE_O " New"))
        this->funcNewScene();

      if (ImGui::MenuItem((char*)ICON_FA_FOLDER_OPEN_O " Open..."))
        this->showOpenDialog = true;

      if (ImGui::BeginMenu((char*)ICON_FA_FILES_O " Open Recent")) {
        if (this->recentFiles.size() == 0)
          ImGui::MenuItem("No recent files", NULL, false, false);
        else {
          for (size_t i = 0; i < this->recentFiles.size(); i++) {
            FBEntity file = this->recentFiles[i];
            if (ImGui::MenuItem(file.title.c_str(), NULL, false, true)) {
              if (std::filesystem::exists(file.path))
                this->funcOpenScene(file);
              else
                this->showRecentFileDoesntExists = true;
            }
          }
          ImGui::Separator();
          if (ImGui::MenuItem("Clear recent files", NULL, false))
            this->recentFilesClear();
        }
        ImGui::EndMenu();
      }

      if (ImGui::MenuItem((char*)ICON_FA_FLOPPY_O " Save..."))
        this->showSaveDialog = true;

      ImGui::Separator();

      if (ImGui::BeginMenu("   Import")) {
        if (ImGui::MenuItem("Wavefront (.OBJ)", NULL, &this->showImporterFile))
          this->dialogImportType = ImportExportFormat_OBJ;
        if (ImGui::MenuItem("glTF (.gltf)", NULL, &this->showImporterFile))
          this->dialogImportType = ImportExportFormat_GLTF;
        if (ImGui::MenuItem("STereoLithography (.STL)", NULL, &this->showImporterFile))
          this->dialogImportType = ImportExportFormat_STL;
        if (ImGui::MenuItem("Stanford (.PLY)", NULL, &this->showImporterFile))
          this->dialogImportType = ImportExportFormat_PLY;
        if (ImGui::BeginMenu("Assimp...")) {
          for (size_t a = 0; a < Settings::Instance()->AssimpSupportedFormats_Import.size(); a++) {
            SupportedAssimpFormat format = Settings::Instance()->AssimpSupportedFormats_Import[a];
            std::string f = std::string(format.description) + " (" + std::string(format.fileExtension) + ")";
            if (ImGui::MenuItem(f.c_str(), NULL, &this->showImporterFile))
              this->dialogImportType_Assimp = static_cast<int>(a);
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu((char*)ICON_FA_FILES_O " Import Recent")) {
        if (this->recentFilesImported.size() == 0)
          ImGui::MenuItem("No recent files", NULL, false, false);
        else {
          for (size_t i = 0; i < this->recentFilesImported.size(); i++) {
            FBEntity file = this->recentFilesImported[i];
            if (ImGui::MenuItem(file.title.c_str(), NULL, false, true)) {
              if (std::filesystem::exists(file.path))
                this->funcProcessImportedFile(file, std::vector<std::string>(), static_cast<ImportExportFormats>(this->dialogImportType), dialogImportType_Assimp);
              else
                this->showRecentFileImportedDoesntExists = true;
            }
          }
          ImGui::Separator();
          if (ImGui::MenuItem("Clear recent files", NULL, false))
            this->recentFilesClearImported();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("   Export")) {
        if (ImGui::MenuItem("Wavefront (.OBJ)", NULL, &this->showExporterFile))
          this->dialogExportType = ImportExportFormat_OBJ;
        if (ImGui::MenuItem("glTF (.gltf)", NULL, &this->showExporterFile))
          this->dialogExportType = ImportExportFormat_GLTF;
        if (ImGui::MenuItem("STereoLithography (.stl)", NULL, &this->showExporterFile))
          this->dialogExportType = ImportExportFormat_STL;
        if (ImGui::MenuItem("Stanford PLY (.ply)", NULL, &this->showExporterFile))
          this->dialogExportType = ImportExportFormat_PLY;
        if (ImGui::BeginMenu("Assimp...")) {
          for (size_t a = 0; a < Settings::Instance()->AssimpSupportedFormats_Export.size(); a++) {
            SupportedAssimpFormat format = Settings::Instance()->AssimpSupportedFormats_Export[a];
            std::string f = std::string(format.description) + " (" + std::string(format.fileExtension) + ")";
            if (ImGui::MenuItem(f.c_str(), NULL, &this->showExporterFile))
              this->dialogExportType_Assimp = static_cast<int>(a);
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenu();
      }

      ImGui::Separator();
#ifdef _WIN32
      if (ImGui::MenuItem((char*)ICON_FA_POWER_OFF " Quit", "Alt+F4"))
#else
      if (ImGui::MenuItem((char*)ICON_FA_POWER_OFF " Quit", "Cmd+Q"))
#endif
        this->funcQuitApp();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Scene")) {
      //            ImGui::MenuItem((char*)ICON_FA_GLOBE " Display Terrain", NULL, &this->showTerrain);
      //            ImGui::Separator();
      if (ImGui::BeginMenu((char*)ICON_FA_LIGHTBULB_O " Add Light")) {
        if (ImGui::MenuItem("Directional (Sun)"))
          this->managerObjects.addLight(LightSourceType_Directional);
        if (ImGui::MenuItem("Point (Light bulb)"))
          this->managerObjects.addLight(LightSourceType_Point);
        if (ImGui::MenuItem("Spot (Flashlight)"))
          this->managerObjects.addLight(LightSourceType_Spot);
        ImGui::EndMenu();
      }
      ImGui::Separator();
      if (ImGui::BeginMenu((char*)ICON_FA_CERTIFICATE " Scene Rendering")) {
        if (ImGui::MenuItem("Solid", NULL, this->managerObjects.viewModelSkin == ViewModelSkin_Solid))
          this->managerObjects.viewModelSkin = ViewModelSkin_Solid;
        if (ImGui::MenuItem("Material", NULL, this->managerObjects.viewModelSkin == ViewModelSkin_Material))
          this->managerObjects.viewModelSkin = ViewModelSkin_Material;
        if (ImGui::MenuItem("Texture", NULL, this->managerObjects.viewModelSkin == ViewModelSkin_Texture))
          this->managerObjects.viewModelSkin = ViewModelSkin_Texture;
        if (ImGui::MenuItem("Wireframe", NULL, this->managerObjects.viewModelSkin == ViewModelSkin_Wireframe))
          this->managerObjects.viewModelSkin = ViewModelSkin_Wireframe;
        if (ImGui::MenuItem("Rendered", NULL, this->managerObjects.viewModelSkin == ViewModelSkin_Rendered))
          this->managerObjects.viewModelSkin = ViewModelSkin_Rendered;
        ImGui::Separator();
        ImGui::MenuItem("Render - Depth", NULL, &this->managerObjects.Setting_Rendering_Depth);
        ImGui::EndMenu();
      }
      ImGui::Separator();
      ImGui::MenuItem((char*)ICON_FA_FILE_IMAGE_O " Render Image", NULL, &this->showImageSave);
      ImGui::MenuItem((char*)ICON_FA_CUBES " Renderer UI", NULL, &this->showRendererUI);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem(this->showControlsGUI ? (char*)ICON_FA_TOGGLE_ON " GUI Controls" : (char*)ICON_FA_TOGGLE_OFF " GUI Controls", NULL, &this->showControlsGUI);
      ImGui::MenuItem(this->showControlsModels ? (char*)ICON_FA_TOGGLE_ON " Scene Controls" : (char*)ICON_FA_TOGGLE_OFF " Scene Controls", NULL, &this->showControlsModels);
      ImGui::MenuItem(Settings::Instance()->showAllVisualArtefacts ? (char*)ICON_FA_TOGGLE_OFF " Hide Visual Artefacts" : (char*)ICON_FA_TOGGLE_ON " Show Visual Artefacts", NULL, &Settings::Instance()->showAllVisualArtefacts);
      ImGui::Separator();
      if (ImGui::MenuItem((char*)ICON_FA_BUG " Show Log Window", NULL, &Settings::Instance()->logDebugInfo))
        Settings::Instance()->saveSettings();
      // ImGui::MenuItem((char*)ICON_FA_PENCIL " Editor", NULL, &this->showShaderEditor);
      if (Settings::Instance()->RendererType == InAppRendererType_Forward)
        ImGui::MenuItem((char*)ICON_FA_PENCIL " IDE", NULL, &this->showKuplungIDE);
      ImGui::MenuItem((char*)ICON_FA_DESKTOP " Screenshot", NULL, &this->showScreenshotWindow);
      ImGui::MenuItem((char*)ICON_FA_TACHOMETER " Scene Statistics", NULL, &this->showSceneStats);
      ImGui::MenuItem((char*)ICON_FA_PAPER_PLANE_O " Structured Volumetric Sampling", NULL, &this->showSVS);
      ImGui::MenuItem((char*)ICON_FA_BICYCLE " Shadertoy", NULL, &this->showShadertoy);
#ifdef DEF_KuplungSetting_UseCuda
      if (Settings::Instance()->UseCuda)
        ImGui::MenuItem((char*)ICON_FA_SPACE_SHUTTLE " Cuda Examples", NULL, &this->showCudaExamples);
#endif
      ImGui::Separator();
      ImGui::MenuItem((char*)ICON_FA_COG " Options", NULL, &this->showOptions);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help")) {
      ImGui::MenuItem((char*)ICON_FA_INFO " Metrics", NULL, &this->showAppMetrics);
      ImGui::MenuItem((char*)ICON_FA_INFO_CIRCLE " About ImGui", NULL, &this->showAboutImgui);
      ImGui::MenuItem((char*)ICON_FA_INFO_CIRCLE " About Kuplung", NULL, &this->showAboutKuplung);
      ImGui::Separator();
      ImGui::MenuItem("   ImGui Demo Window", NULL, &this->showDemoWindow);
      ImGui::EndMenu();
    }

    //ImGui::Text("  --> %.1f FPS | %d vertices, %d indices (%d triangles)", ImGui::GetIO().Framerate, ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
    ImGui::Text("  --> %.1f FPS | %d objs, %d verts, %d indices (%d tris, %d faces) | %s", double(ImGui::GetIO().Framerate), Settings::Instance()->sceneCountObjects, Settings::Instance()->sceneCountVertices, Settings::Instance()->sceneCountIndices, Settings::Instance()->sceneCountTriangles, Settings::Instance()->sceneCountFaces, this->componentConsumption->getOverallStats().c_str());

    ImGui::EndMainMenuBar();
  }

  if (this->showRecentFileDoesntExists)
    this->popupRecentFileDoesntExists();

  if (this->showRecentFileImportedDoesntExists)
    this->popupRecentFileImportedDoesntExists();

  if (this->showSaveDialog)
    this->dialogFileSave(FileSaverOperation_SaveScene);

  if (this->showOpenDialog)
    this->dialogFileSave(FileSaverOperation_OpenScene);

  if (this->showImporterFile)
    this->dialogImporterBrowser();

  if (this->showExporterFile)
    this->dialogExporterBrowser();

  if (this->showDialogStyle)
    this->dialogStyle();

  if (this->showScreenshotWindow)
    this->dialogScreenshot();

  if (this->showShaderEditor)
    this->dialogShaderEditor();

  if (this->showKuplungIDE)
    this->dialogKuplungIDE();

  if (Settings::Instance()->logDebugInfo)
    this->dialogLog();

  if (this->showAppMetrics)
    this->dialogAppMetrics();

  if (this->showAboutImgui)
    this->dialogAboutImGui();

  if (this->showAboutKuplung)
    this->dialogAboutKuplung();

  if (this->showOptions)
    this->dialogOptions(&ImGui::GetStyle());

  if (this->showControlsGUI)
    this->dialogControlsGUI();

  if (this->showControlsModels) // && meshModelFaces != NULL && meshModelFaces->size() > 0)
    this->dialogControlsModels(sceneSelectedModelObject);

  if (this->showSceneStats)
    this->dialogSceneStats();

  if (this->showImageSave)
    this->dialogFileSave(FileSaverOperation_Renderer);

  if (this->showShadertoy)
    this->dialogShadertoy();

  if (this->showShadertoyMessage)
    this->dialogShadertoyMessageWindow();

  if (this->showImageViewer)
    this->componentImageViewer->showImage(&this->showImageViewer);

#ifdef DEF_KuplungSetting_UseCuda
  if (this->showCudaExamples)
    this->dialogCudaExamples();
#endif

  if (this->isParsingOpen)
    ImGui::OpenPopup("Kuplung Parsing");
  if (ImGui::BeginPopupModal("Kuplung Parsing", &this->isParsingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f)));
    ImGui::Text("Processing ... %0.2f%%\n", double(this->parsingPercentage));
    ImGui::ProgressBar(this->parsingPercentage / 100.0f, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::PopStyleColor(1);
    ImGui::EndPopup();
  }

  if (this->isLoadingOpen)
    ImGui::OpenPopup("Kuplung Loading");
  if (ImGui::BeginPopupModal("Kuplung Loading", &this->isLoadingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
    ImGui::Text("Rendering ... \n");
    ImGui::EndPopup();
  }

  if (this->isExportingOpen)
    ImGui::OpenPopup("Kuplung Exporting");
  if (ImGui::BeginPopupModal("Kuplung Exporting", &this->isExportingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, static_cast<ImVec4>(ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f)));
    ImGui::Text("Exporting ... %0.2f%%\n", double(this->parsingPercentage));
    ImGui::ProgressBar(this->parsingPercentage / 100.0f, ImVec2(0.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::PopStyleColor(1);
    ImGui::EndPopup();
  }

  if (this->showDemoWindow)
    ImGui::ShowDemoWindow();
}

void UIManager::clearAllLights() {
  this->controlsGUI->selectedObject = 0;
  this->controlsGUI->selectedObjectLight = -1;
}

void UIManager::showRenderedImage(std::string const& renderedImage) {
  this->componentImageViewer->genTexture = true;
  this->componentImageViewer->imagePath = renderedImage;

  int wWidth, wHeight;
  SDL_GetWindowSize(this->sdlWindow, &wWidth, &wHeight);
  this->componentImageViewer->wWidth = wWidth;
  this->componentImageViewer->wHeight = wHeight;

  this->showImageViewer = true;
}

void UIManager::popupRecentFileDoesntExists() {
  ImGui::OpenPopup("Warning");
  ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("This file no longer exists!");
  if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
    std::vector<FBEntity> recents;
    for (size_t i = 0; i < this->recentFiles.size(); i++) {
      if (std::filesystem::exists(this->recentFiles[i].path))
        recents.push_back(this->recentFiles[i]);
    }
    this->recentFiles = std::move(recents);
    Settings::Instance()->saveRecentFiles(this->recentFiles);
    this->showRecentFileDoesntExists = false;
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

void UIManager::popupRecentFileImportedDoesntExists() {
  ImGui::OpenPopup("Warning");
  ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("This file no longer exists!");
  if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
    std::vector<FBEntity> recents;
    for (size_t i = 0; i < this->recentFilesImported.size(); i++) {
      if (std::filesystem::exists(this->recentFilesImported[i].path))
        recents.push_back(this->recentFilesImported[i]);
    }
    this->recentFilesImported = std::move(recents);
    Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
    this->showRecentFileImportedDoesntExists = false;
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

void UIManager::renderEnd() {
  if (this->needsFontChange) {
    this->windowOptions->loadFonts(&this->needsFontChange);
    ImGui_ImplOpenGL3_CreateFontsTexture();
  }

  ImGui::Render();
}

void UIManager::renderPostEnd() const {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::recentFilesAdd(const FBEntity& file) {
  bool exists = false;
  for (size_t i = 0; i < this->recentFiles.size(); i++) {
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

void UIManager::recentFilesClear() {
  this->recentFiles.clear();
  Settings::Instance()->saveRecentFiles(this->recentFiles);
}

void UIManager::recentFilesAddImported(const FBEntity& file) {
  bool exists = false;
  for (size_t i = 0; i < this->recentFilesImported.size(); i++) {
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

void UIManager::recentFilesClearImported() {
  this->recentFilesImported.clear();
  Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
}

bool UIManager::isMouseOnGUI() const {
  return ImGui::IsAnyWindowHovered();
}

void UIManager::showParsing() {
  this->isParsingOpen = true;
}

void UIManager::hideParsing() {
  this->isParsingOpen = false;
}

void UIManager::showLoading() {
  this->isLoadingOpen = true;
}

void UIManager::hideLoading() {
  this->isLoadingOpen = false;
}

void UIManager::showExporting() {
  this->isExportingOpen = true;
}

void UIManager::hideExporting() {
  this->isExportingOpen = false;
}

void UIManager::dialogFileSave(FileSaverOperation operation) {
  std::string title = "...";
  bool* wType = nullptr;
  assert(operation == FileSaverOperation_SaveScene || operation == FileSaverOperation_OpenScene || operation == FileSaverOperation_Renderer);
  switch (operation) {
    case FileSaverOperation_SaveScene:
      title = "Save Scene";
      wType = &this->showSaveDialog;
      break;
    case FileSaverOperation_OpenScene:
      title = "Open Scene";
      wType = &this->showOpenDialog;
      break;
    case FileSaverOperation_Renderer:
      title = "Render Scene";
      wType = &this->showRenderer;
      break;
  }
  this->componentFileSaver->draw(title.c_str(), operation, wType);
}

void UIManager::dialogImporterBrowser() {
  this->componentImportFile->draw(&this->dialogImportType, &this->dialogImportType_Assimp, &this->showImporterFile);
}

void UIManager::dialogExporterBrowser() {
  this->componentExportFile->draw(&this->dialogExportType, &this->dialogExportType_Assimp, &this->showExporterFile);
}

void UIManager::dialogStyle() {
  this->componentFileBrowser->setStyleBrowser(true);
  this->componentFileBrowser->draw("Open Style", &this->showDialogStyle);
}

void UIManager::dialogScreenshot() {
  this->componentScreenshot->ShowScreenshotsWindow(&this->showScreenshotWindow);
}

void UIManager::dialogShaderEditor() {
  this->componentFileEditor->draw(std::bind(&UIManager::fileShaderEditorSaved, this, std::placeholders::_1), "Editor", &this->showShaderEditor);
}

void UIManager::dialogKuplungIDE() {
  this->componentKuplungIDE->draw("IDE", &this->showKuplungIDE, *this->meshModelFaces, this->managerObjects);
}

void UIManager::dialogLog() {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
  int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
  int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
  this->componentLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
  this->componentLog->draw("Log Window");
}

void UIManager::dialogAppMetrics() {
  ImGui::ShowMetricsWindow(&this->showAppMetrics);
}

void UIManager::dialogAboutImGui() {
  ImGui::SetNextWindowPos(ImVec2(Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2), 0, ImVec2(0.5, 0.5));
  ImGui::Begin("About ImGui", &this->showAboutImgui, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("ImGui %s", ImGui::GetVersion());
  ImGui::Separator();
  ImGui::Text("By Omar Cornut and all github contributors.");
  ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
  ImGui::End();
}

void UIManager::dialogAboutKuplung() {
  ImGui::SetNextWindowPos(ImVec2(Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2), 0, ImVec2(0.5, 0.5));
  ImGui::Begin("About Kuplung", &this->showAboutKuplung, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Kuplung %s", Settings::Instance()->appVersion.c_str());
  ImGui::Separator();
  ImGui::Text("By supudo.net + github.com/supudo");
  ImGui::Text("Whatever license...");
  ImGui::Separator();
  ImGui::Text("Hold mouse wheel to rotate around");
  ImGui::Text("Left Alt + Mouse wheel to increase/decrease the FOV");
  ImGui::Text("Left Shift + Mouse wheel to increase/decrease the FOV");
  ImGui::End();
}

void UIManager::dialogOptions(ImGuiStyle* ref) {
  this->windowOptions->showOptionsWindow(ref, this->windowStyle.get(), &this->showOptions, &this->needsFontChange);
}

void UIManager::dialogSceneStats() {
  int windowWidth, windowHeight;
  SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
  int posX = 10;
  int posY = windowHeight - 200;
  ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_FirstUseEver);
  ImGui::Begin("Scene Stats", &this->showSceneStats, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
  ImGui::Text("OpenGL version: 4.1 (%s)", glGetString(GL_VERSION));
  ImGui::Text("GLSL version: 4.10 (%s)", glGetString(GL_SHADING_LANGUAGE_VERSION));
  ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
  ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
  ImGui::Separator();
  ImGui::Text("Mouse Position: (%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
  ImGui::Separator();
  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
  /// MIGRATION : ImGui::Text("%d allocations", ImGui::GetIO().MetricsAllocs);
  ImGui::End();
}

void UIManager::dialogControlsGUI() {
  this->controlsGUI->render(&this->showControlsGUI, &this->isFrame);
}

void UIManager::dialogControlsModels(int* sceneSelectedModelObject) {
  this->controlsModels->render(&this->showControlsModels, &this->isFrame, this->meshModelFaces, sceneSelectedModelObject);
}

void UIManager::dialogShadertoy() {
  this->componentShadertoy->render(&this->showShadertoy);
}

void UIManager::dialogShadertoyMessage() {
  this->showShadertoyMessage = true;
}

void UIManager::dialogCudaExamples() {
#ifdef DEF_KuplungSetting_UseCuda
  this->componentCudaExamples->draw(&this->showCudaExamples, this->managerObjects.matrixProjection, this->managerObjects.camera->matrixCamera, this->managerObjects.grid->matrixModel);
#endif
}

void UIManager::dialogShadertoyMessageWindow() {
  ImGui::OpenPopup("Paste Error");
  ImGui::BeginPopupModal("Paste Error", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Clipboard size is too big.\nPlease, reduce the shader source and paste it again.");
  if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(), 0))) {
    this->showShadertoyMessage = false;
    ImGui::CloseCurrentPopup();
  }
  ImGui::EndPopup();
}

void UIManager::dialogFileBrowserProcessFile(FBEntity const& file) {
  if (this->showDialogStyle)
    this->windowStyle->load(file.path);
  this->funcProcessImportedFile(file, std::vector<std::string>(), ImportExportFormat_OBJ, -1);
  this->showImporterFile = false;
  this->showDialogFile = false;
  this->showDialogStyle = false;
}

void UIManager::dialogImporterProcessFile(FBEntity const& file, std::vector<std::string> settings, ImportExportFormats importFormat, int importFormatAssimp) {
  this->funcProcessImportedFile(file, settings, importFormat, importFormatAssimp);
  this->showImporterFile = false;
}

void UIManager::dialogExporterProcessFile(FBEntity const& file, std::vector<std::string> settings, ImportExportFormats exportFormat, int exportFormatAssimp) {
  this->funcProcessExportedFile(file, settings, exportFormat, exportFormatAssimp);
  this->showExporterFile = false;
}

void UIManager::dialogFileSaveProcessFile(FBEntity const& file, FileSaverOperation operation) {
  assert(operation == FileSaverOperation_SaveScene || operation == FileSaverOperation_OpenScene || operation == FileSaverOperation_Renderer);
  switch (operation) {
    case FileSaverOperation_SaveScene:
      this->funcSaveScene(file);
      break;
    case FileSaverOperation_OpenScene:
      this->funcOpenScene(file);
      break;
    case FileSaverOperation_Renderer:
      this->funcRenderScene(file);
      break;
  }
  this->showImageSave = false;
  this->showSaveDialog = false;
  this->showOpenDialog = false;
}

void UIManager::fileShaderEditorSaved(std::string const& fileName) {
  this->funcFileShaderCompile(fileName);
}
