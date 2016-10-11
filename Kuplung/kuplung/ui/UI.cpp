//
//  UI.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "UI.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/ui/components/Tabs.hpp"
#include "kuplung/utilities/imgui/imguizmo/ImGuizmo.h"

UI::UI(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
}

UI::~UI() {
    this->destroy();
}

void UI::destroy() {
}

void UI::init(SDL_Window *window,
              std::function<void()> quitApp,
              std::function<void(FBEntity, FileBrowser_ParserType)> processFile,
              std::function<void()> newScene,
              std::function<void(std::string)> fileShaderCompile,
              std::function<void(ShapeType)> addShape,
              std::function<void(LightSourceType)> addLight,
              std::function<void(FBEntity file)> exportScene,
              std::function<void(int)> deleteModel,
              std::function<void(FBEntity file)> renderScene,
              std::function<void(FBEntity file)> saveScene,
              std::function<void(FBEntity file)> openScene
              ) {
    this->sdlWindow = window;
    this->funcQuitApp = quitApp;
    this->funcProcessFile = processFile;
    this->funcNewScene = newScene;
    this->funcFileShaderCompile = fileShaderCompile;
    this->funcAddShape = addShape;
    this->funcAddLight = addLight;
    this->funcExportScene = exportScene;
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
    this->showOBJExporter = false;
    this->showImageSave = false;
    this->showOBJImporter = false;
    this->showSaveDialog = false;
    this->showOpenDialog = false;
    this->showRenderer = false;
    this->showRecentFileDoesntExists = false;
    this->showRecentFileImportedDoesntExists = false;
    this->showSVS = false;
    this->showShadertoy = false;
    this->showShadertoyMessage = false;

    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;

    this->imguiImplementation = std::make_unique<SDL2OpenGL32>();
    this->imguiImplementation->init(this->sdlWindow);

    this->componentLog = std::make_unique<Log>();
    this->componentScreenshot = std::make_unique<Screenshot>();

    this->componentFileBrowser = std::make_unique<FileBrowser>();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UI::dialogOBJImporterProcessFile, this, std::placeholders::_1, std::placeholders::_2));

    this->componentFileSaver = std::make_unique<FileSaver>();
    this->componentFileSaver->init(posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UI::dialogFileSaveProcessFile, this, std::placeholders::_1, std::placeholders::_2));

    this->componentFileEditor = std::make_unique<ShaderEditor>();
    this->componentFileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100);

    this->windowStyle = std::make_unique<DialogStyle>();
    ImGuiStyle& style = ImGui::GetStyle();
    this->windowStyle->saveDefault(style);
    style = this->windowStyle->loadCurrent();

    this->windowOptions = std::make_unique<DialogOptions>();
    this->windowOptions->init();
    this->imguiImplementation->ImGui_Implementation_Init();

    this->windowOptions->loadFonts(&this->needsFontChange);

    this->controlsGUI = std::make_unique<DialogControlsGUI>(this->managerObjects);

    this->controlsModels = std::make_unique<DialogControlsModels>(this->managerObjects);
    this->controlsModels->init(this->sdlWindow, this->funcAddShape, this->funcAddLight, this->funcDeleteModel);

    this->componentSVS = std::make_unique<DialogSVS>();
    this->componentSVS->init();

    this->componentShadertoy = std::make_unique<DialogShadertoy>();
    this->componentShadertoy->init(std::bind(&UI::dialogShadertoyMessage, this));
}

void UI::doLog(std::string const& message) {
    printf("%s\n", message.c_str());
    this->componentLog->addToLog("%s\n", message.c_str());
}

void UI::setSceneSelectedModelObject(int sceneSelectedModelObject) {
    this->controlsModels->selectedObject = sceneSelectedModelObject;
}

bool UI::processEvent(SDL_Event *event) {
    return this->imguiImplementation->ImGui_Implementation_ProcessEvent(event);
}

void UI::renderStart(bool isFrame, int * sceneSelectedModelObject) {
    this->isFrame = isFrame;

    this->imguiImplementation->ImGui_Implementation_NewFrame();
    ImGuizmo::BeginFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE_O " New"))
                this->funcNewScene();

            if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN_O " Open..."))
                this->showOpenDialog = true;

            if (ImGui::BeginMenu(ICON_FA_FILES_O " Open Recent")) {
                if (this->recentFiles.size() == 0)
                    ImGui::MenuItem("No recent files", NULL, false, false);
                else {
                    for (size_t i=0; i<this->recentFiles.size(); i++) {
                        FBEntity file = this->recentFiles[i];
                        if (ImGui::MenuItem(file.title.c_str(), NULL, false, true)) {
                            if (boost::filesystem::exists(file.path))
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

            if (ImGui::MenuItem(ICON_FA_FLOPPY_O " Save..."))
                this->showSaveDialog = true;

            ImGui::Separator();

            if (ImGui::BeginMenu("   Import")) {
                ImGui::MenuItem("Wavefront (.OBJ)", NULL, &this->showOBJImporter);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu(ICON_FA_FILES_O " Import Recent")) {
                if (this->recentFilesImported.size() == 0)
                    ImGui::MenuItem("No recent files", NULL, false, false);
                else {
                    for (size_t i=0; i<this->recentFilesImported.size(); i++) {
                        FBEntity file = this->recentFilesImported[i];
                        if (ImGui::MenuItem(file.title.c_str(), NULL, false, true)) {
                            if (boost::filesystem::exists(file.path)) {
                                FileBrowser_ParserType t;
                                switch (Settings::Instance()->ModelFileParser) {
                                    case 0:
                                        t = FileBrowser_ParserType_Own1;
                                        break;
                                    case 1:
                                        t = FileBrowser_ParserType_Own2;
                                        break;
                                    case 2:
                                        t = FileBrowser_ParserType_Assimp;
                                        break;
                                    default:
                                        t = FileBrowser_ParserType_Assimp;
                                        break;
                                }
                                this->funcProcessFile(file, t);
                            }
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
                ImGui::MenuItem("Wavefront (.OBJ)", NULL, &this->showOBJExporter, true);
                ImGui::EndMenu();
            }

            ImGui::Separator();
#ifdef _WIN32
            if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Alt+F4"))
#else
            if (ImGui::MenuItem(ICON_FA_POWER_OFF " Quit", "Cmd+Q"))
#endif
                this->funcQuitApp();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
//            ImGui::MenuItem(ICON_FA_GLOBE " Display Terrain", NULL, &this->showTerrain);
//            ImGui::Separator();
            if (ImGui::BeginMenu(ICON_FA_LIGHTBULB_O " Add Light")) {
                if (ImGui::MenuItem("Directional (Sun)"))
                    this->managerObjects.addLight(LightSourceType_Directional);
                if (ImGui::MenuItem("Point (Light bulb)"))
                    this->managerObjects.addLight(LightSourceType_Point);
                if (ImGui::MenuItem("Spot (Flashlight)"))
                    this->managerObjects.addLight(LightSourceType_Spot);
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu(ICON_FA_CERTIFICATE " Scene Rendering")) {
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
                ImGui::EndMenu();
            }
            ImGui::Separator();
            ImGui::MenuItem("Render Image", NULL, &this->showImageSave);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem(this->showControlsGUI ? ICON_FA_TOGGLE_ON " GUI Controls" : ICON_FA_TOGGLE_OFF " GUI Controls", NULL, &this->showControlsGUI);
            ImGui::MenuItem(this->showControlsModels ? ICON_FA_TOGGLE_ON " Scene Controls" : ICON_FA_TOGGLE_OFF " Scene Controls", NULL, &this->showControlsModels);
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FA_BUG " Show Log Window", NULL, &Settings::Instance()->logDebugInfo))
                Settings::Instance()->saveSettings();
//            ImGui::MenuItem(ICON_FA_PENCIL " Editor", NULL, &this->showShaderEditor);
            ImGui::MenuItem(ICON_FA_DESKTOP " Screenshot", NULL, &this->showScreenshotWindow);
            ImGui::MenuItem(ICON_FA_TACHOMETER " Scene Statistics", NULL, &this->showSceneStats);
            ImGui::MenuItem(ICON_FA_PAPER_PLANE_O " Structured Volumetric Sampling", NULL, &this->showSVS);
            ImGui::MenuItem(ICON_FA_BICYCLE " Shadertoy", NULL, &this->showShadertoy);
            ImGui::Separator();
            ImGui::MenuItem(ICON_FA_COG " Options", NULL, &this->showOptions);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem(ICON_FA_INFO " Metrics", NULL, &this->showAppMetrics);
            ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About ImGui", NULL, &this->showAboutImgui);
            ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About Kuplung", NULL, &this->showAboutKuplung);
            ImGui::Separator();
            ImGui::MenuItem("   ImGui Demo Window", NULL, &this->showDemoWindow);
            ImGui::EndMenu();
        }

        //ImGui::Text("  --> %.1f FPS | %d vertices, %d indices (%d triangles)", ImGui::GetIO().Framerate, ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
        ImGui::Text("  --> %.1f FPS | %d objs, %d verts, %d indices (%d tris, %d faces)",
                    ImGui::GetIO().Framerate,
                    Settings::Instance()->sceneCountObjects,
                    Settings::Instance()->sceneCountVertices,
                    Settings::Instance()->sceneCountIndices,
                    Settings::Instance()->sceneCountTriangles,
                    Settings::Instance()->sceneCountFaces
                    );

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

    if (this->showOBJImporter)
        this->dialogOBJImporterBrowser();

    if (this->showDialogStyle)
        this->dialogStyle();

    if (this->showScreenshotWindow)
        this->dialogScreenshot();

    if (this->showShaderEditor)
        this->dialogShaderEditor();

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

    if (this->showControlsModels)// && meshModelFaces != NULL && meshModelFaces->size() > 0)
        this->dialogControlsModels(sceneSelectedModelObject);

    if (this->showSceneStats)
        this->dialogSceneStats();

    if (this->showOBJExporter)
        this->dialogFileSave(FileSaverOperation_Exporter);

    if (this->showImageSave)
        this->dialogFileSave(FileSaverOperation_Renderer);

    if (this->showShadertoy)
        this->dialogShadertoy();

    if (this->showShadertoyMessage)
        this->dialogShadertoyMessageWindow();

    if (this->isParsingOpen)
        ImGui::OpenPopup("Kuplung Parsing");
    if (ImGui::BeginPopupModal("Kuplung Parsing", &this->isParsingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
        ImGui::Text("Processing ... %0.2f%%\n", this->parsingPercentage);
        ImGui::ProgressBar(this->parsingPercentage / 100.0, ImVec2(0.0f, 0.0f));
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
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
        ImGui::Text("Exporting ... %0.2f%%\n", this->parsingPercentage);
        ImGui::ProgressBar(this->parsingPercentage / 100.0, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::PopStyleColor(1);
        ImGui::EndPopup();
    }

    if (this->showDemoWindow)
        ImGui::ShowTestWindow(&this->showDemoWindow);
}

void UI::popupRecentFileDoesntExists() {
    ImGui::OpenPopup("Warning");
    ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("This file no longer exists!");
    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) {
        std::vector<FBEntity> recents;
        for (size_t i=0; i<this->recentFiles.size(); i++) {
            if (boost::filesystem::exists(this->recentFiles[i].path))
                recents.push_back(this->recentFiles[i]);
        }
        this->recentFiles = recents;
        Settings::Instance()->saveRecentFiles(this->recentFiles);
        this->showRecentFileDoesntExists = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

void UI::popupRecentFileImportedDoesntExists() {
    ImGui::OpenPopup("Warning");
    ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("This file no longer exists!");
    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) {
        std::vector<FBEntity> recents;
        for (size_t i=0; i<this->recentFilesImported.size(); i++) {
            if (boost::filesystem::exists(this->recentFilesImported[i].path))
                recents.push_back(this->recentFilesImported[i]);
        }
        this->recentFilesImported = recents;
        Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
        this->showRecentFileImportedDoesntExists = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

void UI::renderEnd() {
    if (this->needsFontChange) {
        this->windowOptions->loadFonts(&this->needsFontChange);
        this->imguiImplementation->ImGui_Implementation_CreateFontsTexture();
    }

    ImGui::Render();
    this->imguiImplementation->ImGui_Implementation_RenderDrawLists();
}

void UI::recentFilesAdd(FBEntity file) {
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

void UI::recentFilesClear() {
    this->recentFiles.clear();
    Settings::Instance()->saveRecentFiles(this->recentFiles);
}

void UI::recentFilesAddImported(FBEntity file) {
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

void UI::recentFilesClearImported() {
    this->recentFilesImported.clear();
    Settings::Instance()->saveRecentFilesImported(this->recentFilesImported);
}

bool UI::isMouseOnGUI() {
    return ImGui::IsMouseHoveringAnyWindow();
}

void UI::showParsing() {
    this->isParsingOpen = true;
}

void UI::hideParsing() {
    this->isParsingOpen = false;
}

void UI::showLoading() {
    this->isLoadingOpen = true;
}

void UI::hideLoading() {
    this->isLoadingOpen = false;
}

void UI::showExporting() {
    this->isExportingOpen = true;
}

void UI::hideExporting() {
    this->isExportingOpen = false;
}

#pragma mark - Private Methods

void UI::dialogFileSave(FileSaverOperation operation) {
    std::string title = "";
    bool * wType;
    switch (operation) {
        case FileSaverOperation_SaveScene:
            title = "Save Scene";
            wType = &this->showSaveDialog;
            break;
        case FileSaverOperation_OpenScene:
            title = "Open Scene";
            wType = &this->showOpenDialog;
            break;
        case FileSaverOperation_Exporter:
            title = "Export Scene";
            wType = &this->showOBJExporter;
            break;
        case FileSaverOperation_Renderer:
            title = "Render Scene";
            wType = &this->showRenderer;
            break;
        default:
            title = "...";
            break;
    }
    this->componentFileSaver->draw(title.c_str(), operation, wType);
}

void UI::dialogOBJImporterBrowser() {
    this->componentFileBrowser->setStyleBrowser(false);
    this->componentFileBrowser->draw("Import Wavefront OBJ file", &this->showOBJImporter);
}

void UI::dialogStyle() {
    this->componentFileBrowser->setStyleBrowser(true);
    this->componentFileBrowser->draw("Open Style", &this->showDialogStyle);
}

void UI::dialogScreenshot() {
    this->componentScreenshot->ShowScreenshotsWindow(&this->showScreenshotWindow);
}

void UI::dialogShaderEditor() {
    this->componentFileEditor->draw(std::bind(&UI::fileShaderEditorSaved, this, std::placeholders::_1), "Editor", &this->showShaderEditor);
}

void UI::dialogLog() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
    int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
    this->componentLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
    this->componentLog->draw("Log Window");
}

void UI::dialogAppMetrics() {
    ImGui::ShowMetricsWindow(&this->showAppMetrics);
}

void UI::dialogAboutImGui() {
    ImGui::SetNextWindowPosCenter();
    ImGui::Begin("About ImGui", &this->showAboutImgui, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("ImGui %s", ImGui::GetVersion());
    ImGui::Separator();
    ImGui::Text("By Omar Cornut and all github contributors.");
    ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
    ImGui::End();
}

void UI::dialogAboutKuplung() {
    ImGui::SetNextWindowPosCenter();
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

void UI::dialogOptions(ImGuiStyle* ref) {
    this->windowOptions->showOptionsWindow(ref, this->windowStyle.get(), &this->showOptions, &this->needsFontChange);
}

void UI::dialogSceneStats() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 10;
    int posY = windowHeight - 164;
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Stats", &this->showSceneStats, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("OpenGL version: 4.1 (%s)", glGetString(GL_VERSION));
    ImGui::Text("GLSL version: 4.10 (%s)", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
    ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
    ImGui::Separator();
    ImGui::Text("Mouse Position: (%.1f, %.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
    ImGui::Separator();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("%d vertices, %d indices (%d triangles)", ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);
    ImGui::Text("%d allocations", ImGui::GetIO().MetricsAllocs);
    ImGui::End();
}

void UI::dialogControlsGUI() {
    this->controlsGUI->render(&this->showControlsGUI, &this->isFrame);
}

void UI::dialogControlsModels(int * sceneSelectedModelObject) {
    this->controlsModels->render(&this->showControlsModels, &this->isFrame, this->meshModelFaces, sceneSelectedModelObject);
}

void UI::dialogShadertoy() {
    this->componentShadertoy->render(&this->showShadertoy);
}

void UI::dialogShadertoyMessage() {
    this->showShadertoyMessage = true;
}

void UI::dialogShadertoyMessageWindow() {
    ImGui::OpenPopup("Paste Error");
    ImGui::BeginPopupModal("Paste Error", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Clipboard size is too big.\nPlease, reduce the shader source and paste it again.");
    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) {
        this->showShadertoyMessage = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

void UI::dialogOBJImporterProcessFile(FBEntity file, FileBrowser_ParserType type) {
    if (this->showDialogStyle)
        this->windowStyle->load(file.path);
    this->funcProcessFile(file, type);
    this->showOBJImporter = false;
    this->showDialogFile = false;
    this->showDialogStyle = false;
}

void UI::dialogFileSaveProcessFile(FBEntity file, FileSaverOperation operation) {
    switch (operation) {
        case FileSaverOperation_SaveScene:
            this->funcSaveScene(file);
            break;
        case FileSaverOperation_OpenScene:
            this->funcOpenScene(file);
            break;
        case FileSaverOperation_Exporter:
            this->funcExportScene(file);
            break;
        case FileSaverOperation_Renderer:
            this->funcRenderScene(file);
            break;
        default:
            break;
    }
    this->showOBJExporter = false;
    this->showImageSave = false;
    this->showSaveDialog = false;
    this->showOpenDialog = false;
}

void UI::fileShaderEditorSaved(std::string const& fileName) {
    this->funcFileShaderCompile(fileName);
}
