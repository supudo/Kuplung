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

UI::~UI() {
    this->destroy();
}

void UI::destroy() {
}

void UI::init(SDL_Window *window, ObjectsManager *managerObjects, std::function<void()> quitApp, std::function<void(FBEntity)> processFile, std::function<void()> newScene, std::function<void(std::string)> fileShaderCompile) {
    this->sdlWindow = window;
    this->managerObjects = managerObjects;
    this->funcQuitApp = quitApp;
    this->funcProcessFile = processFile;
    this->funcNewScene = newScene;
    this->funcFileShaderCompile = fileShaderCompile;

    this->isFrame = false;
    this->isLoadingOpen = false;
    this->loadingPercentage = 0.0f;

    this->recentFiles.clear();
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

    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;

    this->imguiImplementation = new SDL2OpenGL32();
    this->imguiImplementation->init(this->sdlWindow);

    this->componentLog = new Log();
    this->componentScreenshot = new Screenshot();

    this->componentFileBrowser = new FileBrowser();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&UI::doLog, this, std::placeholders::_1), std::bind(&UI::dialogFileBrowserProcessFile, this, std::placeholders::_1));

    this->componentFileEditor = new Editor();
    this->componentFileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100, std::bind(&UI::doLog, this, std::placeholders::_1));

    this->windowStyle = new DialogStyle();
    this->windowStyle->init(std::bind(&UI::doLog, this, std::placeholders::_1));
    ImGuiStyle& style = ImGui::GetStyle();
    this->windowStyle->saveDefault(style);
    style = this->windowStyle->loadCurrent();

    this->windowOptions = new DialogOptions();
    this->windowOptions->init(std::bind(&UI::doLog, this, std::placeholders::_1));
    this->imguiImplementation->ImGui_Implementation_Init();

    this->windowOptions->loadFonts(&this->needsFontChange);

    this->controlsGUI = new DialogControlsGUI();
    this->controlsGUI->init(this->managerObjects, std::bind(&UI::doLog, this, std::placeholders::_1));

    this->controlsModels = new DialogControlsModels();
    this->controlsModels->init(this->managerObjects, std::bind(&UI::doLog, this, std::placeholders::_1));
}

void UI::doLog(std::string message) {
    printf("%s\n", message.c_str());
    this->componentLog->addToLog("%s\n", message.c_str());
}

bool UI::processEvent(SDL_Event *event) {
    return this->imguiImplementation->ImGui_Implementation_ProcessEvent(event);
}

void UI::renderStart(bool isFrame) {
    this->isFrame = isFrame;

    this->imguiImplementation->ImGui_Implementation_NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE_O " New"))
                this->funcNewScene();
            ImGui::MenuItem(ICON_FA_FOLDER_OPEN_O " Open ...", NULL, &this->showDialogFile);

            if (ImGui::BeginMenu(ICON_FA_FILES_O " Open Recent")) {
                if (this->recentFiles.size() == 0)
                    ImGui::MenuItem("No recent files", NULL, false, false);
                else {
                    for (std::map<std::string, FBEntity>::iterator iter = this->recentFiles.begin(); iter != this->recentFiles.end(); ++iter) {
                        std::string title = iter->first;
                        FBEntity file = iter->second;
                        if (ImGui::MenuItem(title.c_str(), NULL, false, true))
                            this->funcProcessFile(file);
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Clear recent files", NULL, false))
                        this->recentFilesClear();
                }
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
//            ImGui::MenuItem(ICON_FA_GLOBE " Display Terrain", NULL, &this->showHeightmap);
//            ImGui::Separator();
            if (ImGui::BeginMenu(ICON_FA_LIGHTBULB_O " Add Light")) {
                if (ImGui::MenuItem("Sun"))
                    this->managerObjects->addLight(LightSourceType_Sun);
                if (ImGui::MenuItem("Directional"))
                    this->managerObjects->addLight(LightSourceType_Directional);
                if (ImGui::MenuItem("Point"))
                    this->managerObjects->addLight(LightSourceType_Point);
                if (ImGui::MenuItem("Spot"))
                    this->managerObjects->addLight(LightSourceType_Spot);
                if (ImGui::MenuItem("Hemi"))
                    this->managerObjects->addLight(LightSourceType_Hemi);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            //ImGui::MenuItem(Settings::Instance()->showAxes ? ICON_FA_TOGGLE_ON " Hide Axes" : ICON_FA_TOGGLE_OFF " Show Axes", NULL, &Settings::Instance()->showAxes);
            //ImGui::Separator();
            ImGui::MenuItem(this->showControlsGUI ? ICON_FA_TOGGLE_ON " GUI Controls" : ICON_FA_TOGGLE_OFF " GUI Controls", NULL, &this->showControlsGUI);
            ImGui::MenuItem(this->showControlsModels ? ICON_FA_TOGGLE_ON " Scene Controls" : ICON_FA_TOGGLE_OFF " Scene Controls", NULL, &this->showControlsModels);
            ImGui::Separator();
            ImGui::MenuItem(ICON_FA_BUG " Show Log Window", NULL, &Settings::Instance()->logDebugInfo);
            ImGui::MenuItem(ICON_FA_PENCIL " Editor", NULL, &this->showShaderEditor);
            ImGui::MenuItem(ICON_FA_DESKTOP " Screenshot", NULL, &this->showScreenshotWindow);
            ImGui::MenuItem(ICON_FA_TACHOMETER " Scene Statistics", NULL, &this->showSceneStats);
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

        ImGui::Text("  --> %.1f FPS | %d vertices, %d indices (%d triangles)", ImGui::GetIO().Framerate, ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);

        ImGui::EndMainMenuBar();
    }

    if (this->showDialogFile)
        this->dialogFileBrowser();

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

    if (this->showControlsModels && meshModelFaces->size() > 0)
        this->dialogControlsModels();

    if (this->showSceneStats)
        this->dialogSceneStats();

    if (this->isLoadingOpen)
        ImGui::OpenPopup("Kuplung");
    if (ImGui::BeginPopupModal("Kuplung", &this->isLoadingOpen, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
        ImGui::Text("Processing ... %0.2f%%\n", this->loadingPercentage);
        ImGui::ProgressBar(this->loadingPercentage / 100.0, ImVec2(0.0f, 0.0f));
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::PopStyleColor(1);
        ImGui::EndPopup();
    }

    if (this->showDemoWindow)
        ImGui::ShowTestWindow(&this->showDemoWindow);
}

void UI::renderEnd() {
    if (this->needsFontChange) {
        this->windowOptions->loadFonts(&this->needsFontChange);
        this->imguiImplementation->ImGui_Implementation_CreateFontsTexture();
    }

    ImGui::Render();
    this->imguiImplementation->ImGui_Implementation_RenderDrawLists();
}

void UI::recentFilesAdd(std::string title, FBEntity file) {
    this->recentFiles[title] = file;
}

void UI::recentFilesClear() {
    this->recentFiles.clear();
}

bool UI::isMouseOnGUI() {
    return ImGui::IsMouseHoveringAnyWindow();
}

void UI::showLoading() {
    this->isLoadingOpen = true;
}

void UI::hideLoading() {
    this->isLoadingOpen = false;
}

#pragma mark - Private Methods

void UI::dialogFileBrowser() {
    this->componentFileBrowser->setStyleBrowser(false);
    this->componentFileBrowser->draw("File Browser", &this->showDialogFile);
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
    this->windowOptions->showOptionsWindow(ref, this->windowStyle, &this->showOptions, &this->needsFontChange);
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

void UI::dialogControlsModels() {
    this->controlsModels->render(&this->showControlsModels, &this->isFrame, this->meshModelFaces);
}

void UI::dialogFileBrowserProcessFile(FBEntity file) {
    if (this->showDialogStyle)
        this->windowStyle->load(file.path);
    this->funcProcessFile(file);
    this->showDialogFile = false;
    this->showDialogStyle = false;
}

void UI::fileShaderEditorSaved(std::string fileName) {
    this->funcFileShaderCompile(fileName);
}

