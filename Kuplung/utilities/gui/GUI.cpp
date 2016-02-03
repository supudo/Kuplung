//
//  gui.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "utilities/gl/GLIncludes.h"
#include <SDL2/SDL_syswm.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include "utilities/settings/Settings.h"
#include "GUI.hpp"
#include "IconsFontAwesome.h"
#include "IconsMaterialDesign.h"
#include "GUITabs.hpp"

#pragma mark - Destructor

GUI::~GUI() {
    this->destroy();
}

#pragma mark - Public

void GUI::init(SDL_Window *window, std::function<void()> quitApp, std::function<void(FBEntity)> processFile, std::function<void()> newScene) {
    this->isGUIVisible = false;
    this->sdlWindow = window;
    this->quitApp = quitApp;
    this->processFile = processFile;
    this->newScene = newScene;

    this->isLoadingOpen = false;
    this->showScreenshotWindow = false;
    this->logDebugInfo = true;
    this->showFileDialog = false;
    this->showStyleDialog = false;
    this->showAppMetrics = false;
    this->showAboutKuplung = false;
    this->showAboutImgui = false;
    this->displaySceneStats = false;
    this->showHeightmap = false;
    this->showEditor = false;
    this->newHeightmap = false;
    this->outlineColorPickerOpen = false;
    this->cmenu_deleteYn = false;
    this->cmenu_renameModel = false;

    this->sceneLights = {};

    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;

    this->windowLog = new GUILog();
    this->windowScreenshot = new GUIScreenshot();

    this->windowFileBrowser = new GUIFileBrowser();
    this->windowFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&GUI::doLog, this, std::placeholders::_1), std::bind(&GUI::dialogFileBrowserProcessFile, this, std::placeholders::_1));

    this->colorPicker = new GUIColorPicker();

    this->guiStyle = new GUIStyle();
    this->guiStyle->init(std::bind(&GUI::doLog, this, std::placeholders::_1));
    ImGuiStyle& style = ImGui::GetStyle();
    this->guiStyle->saveDefault(style);
    style = this->guiStyle->loadCurrent();

    this->fileEditor = new GUIEditor();
    this->fileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100, std::bind(&GUI::doLog, this, std::placeholders::_1));

    this->gui_item_selected = -1;
    this->scene_item_selected = -1;
    this->selectedTabScene = 0;
    this->selectedTabGUICamera = 0;
    this->selectedTabGUIGrid = 0;
    this->selectedTabGUILight = 0;
    this->selectedTabGUITerrain = 0;

    this->isFrame = false;
    this->isProjection = true;
    this->fixedGridWorld = true;

    // icon font
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

    // http://fortawesome.github.io/Font-Awesome/icons/
    std::string faFont = Settings::Instance()->appFolder() + "/fonts/fontawesome-webfont.ttf";
    static const ImWchar fa_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig fa_config;
    fa_config.MergeMode = true;
    fa_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(faFont.c_str(), 14.0f, &fa_config, fa_ranges);

    // https://design.google.com/icons/
    std::string gmFont = Settings::Instance()->appFolder() + "/fonts/material-icons-regular.ttf";
    static const ImWchar gm_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
    ImFontConfig gm_config;
    gm_config.MergeMode = true;
    gm_config.PixelSnapH = true;
    io.Fonts->AddFontFromFileTTF(gmFont.c_str(), 22.0f, &gm_config, gm_ranges);

    if (Settings::Instance()->OpenGLMajorVersion > 2)
        this->ImGui_SDL2GL32_Implementation_Init();
    else
        this->ImGui_SDL2GL21_Implementation_Init();
}

void GUI::setContextMenuModel(std::function<void(int)> deleteModel, std::function<void(int, std::string)> renameModel) {
    this->contextMenuDeleteModelFunc = deleteModel;
    this->contextMenuRenameModelFunc = renameModel;
}

bool GUI::processEvent(SDL_Event *event) {
    if (Settings::Instance()->OpenGLMajorVersion > 2)
        return this->ImGui_SDL2GL32_Implementation_ProcessEvent(event);
    else
        return this->ImGui_SDL2GL21_Implementation_ProcessEvent(event);
}

void GUI::destroy() {
    for (int i=0; i<(int)this->scene_item_settings.size(); i++) {
        std::vector<GUIObjectSetting*> setts = this->scene_item_settings.at(i);
        for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
            delete *settObj;
        }
    }

    for (int i=0; i<(int)this->scene_item_settings_default.size(); i++) {
        std::vector<GUIObjectSetting*> setts = this->scene_item_settings_default.at(i);
        for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
            delete *settObj;
        }
    }

    for (int i=0; i<(int)this->gui_item_settings.size(); i++) {
        std::vector<GUIObjectSetting*> setts = this->gui_item_settings.at(i);
        for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
            delete *settObj;
        }
    }

    for (int i=0; i<(int)this->gui_item_settings_default.size(); i++) {
        std::vector<GUIObjectSetting*> setts = this->gui_item_settings_default.at(i);
        for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
            delete *settObj;
        }
    }

    this->scene_item_settings.clear();
    this->scene_item_settings_default.clear();
    this->gui_item_settings.clear();
    this->gui_item_settings_default.clear();

    if (Settings::Instance()->OpenGLMajorVersion > 2)
        this->ImGui_SDL2GL32_Implementation_Shutdown();
    else
        this->ImGui_SDL2GL21_Implementation_Shutdown();
}

void GUI::doLog(std::string logMessage) {
    printf("%s\n", logMessage.c_str());
    this->windowLog->addToLog("%s\n", logMessage.c_str());
}

void GUI::recentFilesAdd(std::string title, FBEntity file) {
    this->recentFiles[title] = file;
}

void GUI::recentFilesClear() {
    this->recentFiles.clear();
}

void GUI::initGUIControls(int guiObjectsCount, std::vector<std::vector<float>> initialSettings) {
    // general settings holder
    std::vector<GUIObjectSetting*> setts;
    std::vector<GUIObjectSetting*> setts_default;
    for (int j=0; j<22; j++) {
        GUIObjectSetting* gos = new GUIObjectSetting();
        gos->oIndex = j;
        gos->oAnimate = false;
        gos->bValue = true;
        gos->fValue = 0.0;
        setts.push_back(gos);

        GUIObjectSetting* gos_default = new GUIObjectSetting();
        gos_default->oIndex = j;
        gos_default->oAnimate = false;
        gos_default->bValue = true;
        gos_default->fValue = 0.0;
        setts_default.push_back(gos_default);
    }
    this->gui_item_settings.push_back(setts);
    this->gui_item_settings_default.push_back(setts_default);

    // GUI objects
    for (int i=0; i<guiObjectsCount; i++) {
        std::vector<GUIObjectSetting*> setts;
        std::vector<GUIObjectSetting*> setts_default;
        for (int j=0; j<22; j++) {
            GUIObjectSetting* gos = new GUIObjectSetting();
            gos->oIndex = j;
            gos->oAnimate = false;
            gos->bValue = true;
            gos->fValue = initialSettings[i][j];
            setts.push_back(gos);

            GUIObjectSetting* gos_default = new GUIObjectSetting();
            gos_default->oIndex = j;
            gos_default->oAnimate = false;
            gos_default->bValue = true;
            gos_default->fValue = initialSettings[i][j];
            setts_default.push_back(gos_default);
        }
        this->gui_item_settings.push_back(setts);
        this->gui_item_settings_default.push_back(setts_default);
    }
}

void GUI::showGUIControls() {
    this->displayGUIControls = true;
}

void GUI::hideGUIControls() {
    this->displayGUIControls = true;
}

void GUI::removeSceneModelSettings(int idx) {
    for (int i=0; i<(int)this->scene_item_settings.size(); i++) {
        if (i == idx) {
            std::vector<GUIObjectSetting*> setts = this->scene_item_settings.at(i);
            for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
                delete *settObj;
            }
            this->scene_item_settings.erase(this->scene_item_settings.begin() + idx);
            break;
        }
    }

    for (int i=0; i<(int)this->scene_item_settings_default.size(); i++) {
        if (i == idx) {
            std::vector<GUIObjectSetting*> setts = this->scene_item_settings_default.at(i);
            for (std::vector<GUIObjectSetting*>::iterator settObj = setts.begin(); settObj != setts.end(); ++settObj) {
                delete *settObj;
            }
            this->scene_item_settings_default.erase(this->scene_item_settings_default.begin() + idx);
            break;
        }
    }

    if ((*this->meshModelFaces).size() == 0) {
        this->hideSceneSettings();
        this->hideSceneStats();
    }
}

void GUI::addSceneModelSettings() {
    std::vector<GUIObjectSetting*> setts = {};
    std::vector<GUIObjectSetting*> setts_default = {};

    int idx = 0;

    // scale
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // rotate
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // translate
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // displacement
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // refraction
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // material
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 0.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 0.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 0.0)); idx += 1;
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 0.0)); idx += 1;

    // specular exp
    setts.push_back(this->addSceneSettingsObject(idx, 0.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 0.0)); idx += 1;

    // illumination model
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0)); idx += 1;

    // cel-shading
    setts.push_back(this->addSceneSettingsObject(idx, 1.0, false));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0, false)); idx += 1;

    // alpha
    setts.push_back(this->addSceneSettingsObject(idx, 1.0));
    setts_default.push_back(this->addSceneSettingsObject(idx, 1.0));

    this->scene_item_settings.push_back(setts);
    this->scene_item_settings_default.push_back(setts_default);
}

GUIObjectSetting* GUI::addSceneSettingsObject(int idx, float fValue, bool bValue, glm::vec4 vValue) {
    GUIObjectSetting *gos = new GUIObjectSetting();
    gos->oIndex = idx;
    gos->oAnimate = false;
    gos->fValue = fValue;
    gos->bValue = bValue;
    gos->vValue = vValue;
    return gos;
}

void GUI::setModelSetting(int modelID, int settingID, int iValue, float fValue, bool bValue, glm::vec4 vValue) {
    this->scene_item_settings[modelID][settingID]->iValue = iValue;
    this->scene_item_settings[modelID][settingID]->fValue = fValue;
    this->scene_item_settings[modelID][settingID]->bValue = bValue;
    this->scene_item_settings[modelID][settingID]->vValue = vValue;
    this->scene_item_settings_default[modelID][settingID]->iValue = iValue;
    this->scene_item_settings_default[modelID][settingID]->fValue = fValue;
    this->scene_item_settings_default[modelID][settingID]->bValue = bValue;
    this->scene_item_settings_default[modelID][settingID]->vValue = vValue;
}

void GUI::addSceneLight() {
    GUILightObject *glo_ambient = new GUILightObject();
    glo_ambient->colorPickerOpen = false;
    glo_ambient->color = glm::vec3(1, 1, 1);
    glo_ambient->strength = 1.0;

    GUILightObject *glo_diffuse = new GUILightObject();
    glo_diffuse->colorPickerOpen = false;
    glo_diffuse->color = glm::vec3(1, 1, 1);
    glo_diffuse->strength = 1.0;

    GUILightObject *glo_specular = new GUILightObject();
    glo_specular->colorPickerOpen = false;
    glo_specular->color = glm::vec3(1, 1, 1);
    glo_specular->strength = 0.0;

    GUISceneLight *gsl = new GUISceneLight();
    gsl->ambient = glo_ambient;
    gsl->diffuse = glo_diffuse;
    gsl->specular = glo_specular;

    this->sceneLights.push_back(gsl);
}

void GUI::hideSceneSettings() {
    this->displaySceneSettings = false;
}

void GUI::showSceneStats() {
    this->displaySceneStats = true;
}

void GUI::hideSceneStats() {
    this->displaySceneStats = false;
}

void GUI::showLoading() {
    this->isLoadingOpen = true;
}

void GUI::hideLoading() {
    this->isLoadingOpen = false;
}

void GUI::setHeightmapImage(std::string heightmapImage) {
    if (this->heightmapImage != heightmapImage) {
        this->heightmapImage = heightmapImage;
        this->newHeightmap = true;
    }
}

void GUI::setShaderEditor(std::function<void(std::string)> fileShaderCompile) {
    this->doFileShaderCompile = fileShaderCompile;
}

bool GUI::isMouseOnGUI() {
    return ImGui::IsMouseHoveringAnyWindow();
}

#pragma mark - Rendering

void GUI::renderStart(bool isFrame) {
    this->isFrame = isFrame;

    if (Settings::Instance()->OpenGLMajorVersion > 2)
        this->ImGui_SDL2GL32_Implementation_NewFrame();
    else
        this->ImGui_SDL2GL21_Implementation_NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem(ICON_FA_FILE_O " New"))
                this->newScene();
            ImGui::MenuItem(ICON_FA_FOLDER_OPEN_O " Open ...", NULL, &showFileDialog);

            if (ImGui::BeginMenu(ICON_FA_FILES_O " Open Recent")) {
                if (this->recentFiles.size() == 0)
                    ImGui::MenuItem("No recent files", NULL, false, false);
                else {
                    for (std::map<std::string, FBEntity>::iterator iter = this->recentFiles.begin(); iter != this->recentFiles.end(); ++iter) {
                        std::string title = iter->first;
                        FBEntity file = iter->second;
                        if (ImGui::MenuItem(title.c_str(), NULL, false, true))
                            this->processFile(file);
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
                this->quitApp();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            ImGui::MenuItem(ICON_FA_GLOBE " Display Terrain", NULL, &this->showHeightmap);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem(Settings::Instance()->showGrid ? ICON_FA_TOGGLE_ON " Hide Grid" : ICON_FA_TOGGLE_OFF " Show Grid", NULL, &Settings::Instance()->showGrid);
            ImGui::MenuItem(Settings::Instance()->showLight ? ICON_FA_TOGGLE_ON " Hide Light" : ICON_FA_TOGGLE_OFF " Show Light", NULL, &Settings::Instance()->showLight);
            ImGui::MenuItem(Settings::Instance()->showAxes ? ICON_FA_TOGGLE_ON " Hide Axes" : ICON_FA_TOGGLE_OFF " Show Axes", NULL, &Settings::Instance()->showAxes);
            ImGui::Separator();
            ImGui::MenuItem(this->displayGUIControls ? ICON_FA_TOGGLE_ON " GUI Controls" : ICON_FA_TOGGLE_OFF " GUI Controls", NULL, &this->displayGUIControls);
            ImGui::MenuItem(this->displaySceneSettings ? ICON_FA_TOGGLE_ON " Scene Controls" : ICON_FA_TOGGLE_OFF " Scene Controls", NULL, &this->displaySceneSettings);
            ImGui::Separator();
            ImGui::MenuItem(ICON_FA_BUG " Show Log Window", NULL, &Settings::Instance()->logDebugInfo);
            ImGui::MenuItem(ICON_FA_PENCIL " Editor", NULL, &this->showEditor);
            ImGui::MenuItem(ICON_FA_DESKTOP " Screenshot", NULL, &this->showScreenshotWindow);
            ImGui::MenuItem(ICON_FA_TACHOMETER " Scene Statistics", NULL, &this->displaySceneStats);
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

    if (this->showFileDialog)
        this->dialogFileBrowser();

    if (this->showStyleDialog)
        this->dialogStyleBrowser();

    if (this->showScreenshotWindow)
        this->dialogScreenshot();

    if (this->showEditor)
        this->dialogEditor();

    if (Settings::Instance()->logDebugInfo)
        this->dialogLog();

    if (this->showAppMetrics)
        this->dialogMetrics();

    if (this->showAboutImgui)
        this->dialogAboutImGui();

    if (this->showAboutKuplung)
        this->dialogAboutKuplung();

    if (this->showOptions)
        this->dialogOptions(&ImGui::GetStyle());

    if (this->displayGUIControls)
        this->dialogGUIControls();

    if (this->displaySceneSettings)
        this->dialogSceneSettings();

    if (this->displaySceneStats)
        this->dialogSceneStats();

    if (this->showHeightmap)
        this->dialogHeightmap();

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

void GUI::renderEnd() {
    ImGui::Render();
    this->ImGui_Implementation_RenderDrawLists();
}

#pragma mark - Dialogs

void GUI::dialogFileBrowser() {
    this->windowFileBrowser->setStyleBrowser(false);
    this->windowFileBrowser->draw("File Browser", &this->showFileDialog);
}

void GUI::dialogStyleBrowser() {
    this->windowFileBrowser->setStyleBrowser(true);
    this->windowFileBrowser->draw("Open Style", &this->showStyleDialog);
}

void GUI::dialogFileBrowserProcessFile(FBEntity file) {
    if (this->showStyleDialog)
        this->guiStyle->load(file.path);
    this->processFile(file);
    this->showFileDialog = false;
    this->showStyleDialog = false;
}

void GUI::dialogScreenshot() {
    this->windowScreenshot->ShowScreenshotsWindow(&this->showScreenshotWindow);
}

void GUI::dialogEditor() {
    this->fileEditor->draw(std::bind(&GUI::fileEditorSaved, this, std::placeholders::_1), "Editor", &this->showEditor);
}

void GUI::fileEditorSaved(std::string fileName) {
    this->doFileShaderCompile(fileName);
}

void GUI::dialogLog() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
    int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
    this->windowLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
    this->windowLog->draw("Log Window");
}

void GUI::dialogMetrics() {
    ImGui::ShowMetricsWindow(&this->showAppMetrics);
}

void GUI::dialogAboutImGui() {
    ImGui::SetNextWindowPosCenter();
    ImGui::Begin("About ImGui", &this->showAboutImgui, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("ImGui %s", ImGui::GetVersion());
    ImGui::Separator();
    ImGui::Text("By Omar Cornut and all github contributors.");
    ImGui::Text("ImGui is licensed under the MIT License, see LICENSE for more information.");
    ImGui::End();
}

void GUI::dialogAboutKuplung() {
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

void GUI::dialogOptions(ImGuiStyle* ref) {
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Options", &this->showOptions, ImGuiWindowFlags_ShowBorders);

    if (ImGui::TreeNode("General")) {
        if (ImGui::Checkbox("Log Messages", &Settings::Instance()->logDebugInfo))
            Settings::Instance()->saveSettings();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Style")) {
        ImGuiStyle& style = ImGui::GetStyle();

        const ImGuiStyle def;
        if (ImGui::Button("Default")) {
            style = ref ? *ref : def;
            style = this->guiStyle->loadDefault();
            this->guiStyle->save(style);
        }
        if (ref) {
            ImGui::SameLine();
            if (ImGui::Button("Save")) {
                *ref = style;
                this->guiStyle->save(style);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
            this->showStyleDialog = true;

        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.55f);

        if (ImGui::TreeNode("Rendering")) {
            ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
            ImGui::Checkbox("Anti-aliased shapes", &style.AntiAliasedShapes);
            ImGui::PushItemWidth(100);
            ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, FLT_MAX, NULL, 2.0f);
            if (style.CurveTessellationTol < 0.0f)
                style.CurveTessellationTol = 0.10f;
            ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
            ImGui::DragFloat("Window Fill Alpha Default", &style.WindowFillAlphaDefault, 0.005f, 0.0f, 1.0f, "%.2f");
            ImGui::PopItemWidth();
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Sizes")) {
            ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat("ChildWindowRounding", &style.ChildWindowRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 16.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 16.0f, "%.0f");
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Colors")) {
            static ImGuiColorEditMode edit_mode = ImGuiColorEditMode_RGB;
            ImGui::RadioButton("RGB", &edit_mode, ImGuiColorEditMode_RGB);
            ImGui::SameLine();
            ImGui::RadioButton("HSV", &edit_mode, ImGuiColorEditMode_HSV);
            ImGui::SameLine();
            ImGui::RadioButton("HEX", &edit_mode, ImGuiColorEditMode_HEX);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", 200);

            ImGui::BeginChild("#colors", ImVec2(0, 300), true);
            ImGui::PushItemWidth(-160);
            ImGui::ColorEditMode(edit_mode);
            for (int i = 0; i < ImGuiCol_COUNT; i++) {
                const char* name = ImGui::GetStyleColName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4(name, (float*)&style.Colors[i], true);
                if (memcmp(&style.Colors[i], (ref ? &ref->Colors[i] : &def.Colors[i]), sizeof(ImVec4)) != 0) {
                    ImGui::SameLine(); if (ImGui::Button("Revert")) style.Colors[i] = ref ? ref->Colors[i] : def.Colors[i];
                    if (ref) { ImGui::SameLine(); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i]; }
                }
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::TreePop();
        }

        ImGui::PopItemWidth();
        ImGui::TreePop();
    }

    ImGui::End();
}

void GUI::dialogHeightmap() {
    ImGui::Begin("Heightmap", &this->showHeightmap, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders);

    if (this->newHeightmap) {
        int tChannels;
        unsigned char* tPixels = stbi_load(this->heightmapImage.c_str(), &this->heightmapWidth, &this->heightmapHeight, &tChannels, 0);
        if (!tPixels)
            this->doLog("Can't load diffuse texture image - " + this->heightmapImage + " with error - " + std::string(stbi_failure_reason()));
        else {
            glGenTextures(1, &this->vboTexHeightmap);
            glBindTexture(GL_TEXTURE_2D, this->vboTexHeightmap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->heightmapWidth, this->heightmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(tPixels);
        }
    }
    ImGui::Image((ImTextureID)(intptr_t)this->vboTexHeightmap, ImVec2(this->heightmapWidth, this->heightmapHeight));

    ImGui::End();

    this->newHeightmap = false;
}

#pragma mark - Scene GUI dialogs

void GUI::dialogGUIControls() {
    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("GUI Controls", &this->displayGUIControls, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->resetValuesGUIControls();
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);

    const char* gui_items[] = { "General", "Camera", "Grid", "Light", "Terrain" };
    //ImGui::Combo("##111", &this->gui_item_selected, gui_items, IM_ARRAYSIZE(gui_items));
    //ImGui::Text("GUI Objects");
    ImGui::ListBox("", &this->gui_item_selected, gui_items, IM_ARRAYSIZE(gui_items));
    ImGui::PopItemWidth();

    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
    switch (this->gui_item_selected) {
        case 0: {
            this->addControlsSlider("Field of view", 1, true, 0.0f, 180.0f, false, NULL, &this->so_GUI_FOV);
            ImGui::Separator();

            ImGui::Text("Ratio"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("W & H");
            ImGui::SliderFloat("W##105", &this->so_GUI_ratio_w, 0.0f, 5.0f);
            ImGui::SliderFloat("H##106", &this->so_GUI_ratio_h, 0.0f, 5.0f);
            ImGui::Separator();

            ImGui::Text("Planes"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Far & Close");
            ImGui::SliderFloat("Far##107", &this->so_GUI_plane_close, 0.0f, 1.0f);
            ImGui::SliderFloat("Close##108", &this->so_GUI_plane_far, 0.0f, 100.0f);
            ImGui::Separator();

            ImGui::Text("Grid size"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Squares");
            ImGui::SliderInt("##109", &this->so_GUI_grid_size, 0, 100);
            ImGui::Separator();

            ImGui::Checkbox("Grid fixed with World", &this->fixedGridWorld);
            ImGui::Separator();

            this->addControlColor4("Outline Color", &this->so_GUI_outlineColor, &this->outlineColorPickerOpen);
            this->addControlsSlider("Outline Thickness", 0, true, 1.01f, 2.0f, false, NULL, &this->so_outlineThickness);
            break;
        }
        case 1: {
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

            const char* tabsGUICamera[] = {
                "\n" ICON_MD_REMOVE_RED_EYE,
                "\n" ICON_MD_3D_ROTATION,
                "\n" ICON_MD_OPEN_WITH,
            };
            const char* tabsLabelsGUICamera[] = { "Look At", "Rotate", "Translate" };
            const int numTabsGUICamera = sizeof(tabsGUICamera) / sizeof(tabsGUICamera[0]);
            ImGui::TabLabels(numTabsGUICamera, tabsGUICamera, this->selectedTabGUICamera, ImVec2(30.0, 30.0), tabsLabelsGUICamera);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (this->selectedTabGUICamera) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Look-At matrix");
                    ImGui::SliderFloat("Eye X", &this->gui_item_settings[this->gui_item_selected][0]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Eye Y", &this->gui_item_settings[this->gui_item_selected][1]->fValue, -100.0f, 10.0f);
                    ImGui::SliderFloat("Eye Z", &this->gui_item_settings[this->gui_item_selected][2]->fValue, 0.0f, 90.0f);
                    ImGui::Separator();
                    ImGui::SliderFloat("Center X", &this->gui_item_settings[this->gui_item_selected][3]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Center Y", &this->gui_item_settings[this->gui_item_selected][4]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Center Z", &this->gui_item_settings[this->gui_item_selected][5]->fValue, 0.0f, 45.0f);
                    ImGui::Separator();
                    ImGui::SliderFloat("Up X", &this->gui_item_settings[this->gui_item_selected][6]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Up Y", &this->gui_item_settings[this->gui_item_selected][7]->fValue, -1.0f, 1.0f);
                    ImGui::SliderFloat("Up Z", &this->gui_item_settings[this->gui_item_selected][8]->fValue, -10.0f, 10.0f);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][12]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 12, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##2", &this->gui_item_settings[this->gui_item_selected][12]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][13]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 13, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##2", &this->gui_item_settings[this->gui_item_selected][13]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][14]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 14, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##2", &this->gui_item_settings[this->gui_item_selected][14]->fValue, 0.0f, 360.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][15]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 15, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##3", &this->gui_item_settings[this->gui_item_selected][15]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][16]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 16, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##3", &this->gui_item_settings[this->gui_item_selected][16]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][17]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 17, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##3", &this->gui_item_settings[this->gui_item_selected][17]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case 2: {
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

            const char* tabsGUIGrid[] = {
                "\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
                "\n" ICON_MD_3D_ROTATION,
                "\n" ICON_MD_OPEN_WITH,
            };
            const char* tabsLabelsGUIGrid[] = { "Scale", "Rotate", "Translate" };
            const int numTabsGUIGrid = sizeof(tabsGUIGrid) / sizeof(tabsGUIGrid[0]);
            ImGui::TabLabels(numTabsGUIGrid, tabsGUIGrid, this->selectedTabGUIGrid, ImVec2(30.0, 30.0), tabsLabelsGUIGrid);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (this->selectedTabGUIGrid) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    ImGui::SliderFloat("X##1", &this->gui_item_settings[this->gui_item_selected][9]->fValue, 0.0f, 1.0f);
                    ImGui::SliderFloat("Y##1", &this->gui_item_settings[this->gui_item_selected][10]->fValue, 0.0f, 1.0f);
                    ImGui::SliderFloat("Z##1", &this->gui_item_settings[this->gui_item_selected][11]->fValue, 0.0f, 1.0f);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][12]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 12, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##2", &this->gui_item_settings[this->gui_item_selected][12]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][13]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 13, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##2", &this->gui_item_settings[this->gui_item_selected][13]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][14]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 14, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##2", &this->gui_item_settings[this->gui_item_selected][14]->fValue, 0.0f, 360.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][15]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 15, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##3", &this->gui_item_settings[this->gui_item_selected][15]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][16]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 16, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##3", &this->gui_item_settings[this->gui_item_selected][16]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][17]->oAnimate))
                        this->animateValue(true, this->gui_item_selected, 17, 0.05f, this->so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##3", &this->gui_item_settings[this->gui_item_selected][17]->fValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case 3: {
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

            const char* tabsGUILight[] = {
                "\n" ICON_MD_TRANSFORM,
                "\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
                "\n" ICON_MD_3D_ROTATION,
                "\n" ICON_MD_OPEN_WITH,
                "\n" ICON_MD_COLOR_LENS,
            };
            const char* tabsLabelsGUILight[] = { "General", "Scale", "Rotate", "Translate", "Colors" };
            const int numTabsGUILight = sizeof(tabsGUILight) / sizeof(tabsGUILight[0]);
            ImGui::TabLabels(numTabsGUILight, tabsGUILight, this->selectedTabGUILight, ImVec2(30.0, 30.0), tabsLabelsGUILight);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (this->selectedTabGUILight) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Properties");
                    // show lamp object
                    ImGui::Checkbox("Show Lamp", &Settings::Instance()->showLight);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    this->addControlsXYZ(true, 9, 10, 11, "scale", 0.01f, 1.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    this->addControlsXYZ(true, 12, 13, 14, "rotation", 1.0f, 360.0f);
                    break;
                }
                case 3: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    this->addControlsXYZ(true, 15, 16, 17, "translation", 0.05f, this->so_GUI_grid_size);
                    break;
                }
                case 4: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Light colors");

                    this->addControlColor3("Ambient Color", &this->sceneLights[0]->ambient->color, &this->sceneLights[0]->ambient->colorPickerOpen);
                    this->addControlsSlider("Ambient Strength", 18, true, 0.1f, 1.0f, true, &this->gui_item_settings[this->gui_item_selected][18]->oAnimate, &this->gui_item_settings[this->gui_item_selected][18]->fValue);

                    this->addControlColor3("Diffuse Color", &this->sceneLights[0]->diffuse->color, &this->sceneLights[0]->diffuse->colorPickerOpen);
                    this->addControlsSlider("Diffuse Strength", 19, true, 0.1f, 4.0f, true, &this->gui_item_settings[this->gui_item_selected][19]->oAnimate, &this->gui_item_settings[this->gui_item_selected][19]->fValue);

                    this->addControlColor3("Specular Color", &this->sceneLights[0]->specular->color, &this->sceneLights[0]->specular->colorPickerOpen);
                    this->addControlsSlider("Specular Strength", 20, true, 0.1f, 4.0f, true, &this->gui_item_settings[this->gui_item_selected][20]->oAnimate, &this->gui_item_settings[this->gui_item_selected][20]->fValue);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case 4: {
            ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

            const char* tabsGUITerrain[] = {
                "\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
                "\n" ICON_MD_3D_ROTATION,
                "\n" ICON_MD_OPEN_WITH,
            };
            const char* tabsLabelsGUITerrain[] = { "Scale", "Rotate", "Translate" };
            const int numTabsGUITerrain = sizeof(tabsGUITerrain) / sizeof(tabsGUITerrain[0]);
            ImGui::TabLabels(numTabsGUITerrain, tabsGUITerrain, this->selectedTabGUITerrain, ImVec2(30.0, 30.0), tabsLabelsGUITerrain);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (this->selectedTabGUITerrain) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    this->addControlsXYZ(true, 9, 10, 11, "rotation", 0.01f, 1.0f);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    this->addControlsXYZ(true, 12, 13, 14, "rotation", 1.0f, 360.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    this->addControlsXYZ(true, 15, 16, 17, "rotation", 1.0f, 360.0f);
                    break;
                }
                default:
                    break;
            }
            break;
        }
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void GUI::addControlsXYZ(bool isGuiControl, int x, int y, int z, std::string animate, float animateStep, float animateLimit) {
    if (isGuiControl) {
        if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][x]->oAnimate))
            this->animateValue(true, this->gui_item_selected, x, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by X", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("X##2", &this->gui_item_settings[this->gui_item_selected][x]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][y]->oAnimate))
            this->animateValue(true, this->gui_item_selected, y, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Y", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Y##2", &this->gui_item_settings[this->gui_item_selected][y]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][z]->oAnimate))
            this->animateValue(true, this->gui_item_selected, z, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Z", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Z##2", &this->gui_item_settings[this->gui_item_selected][z]->fValue, 0.0f, animateLimit);
    }
    else {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][x]->oAnimate))
            this->animateValue(false, this->scene_item_selected, x, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by X", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("X##101", &this->scene_item_settings[this->scene_item_selected][x]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##2", &this->scene_item_settings[this->scene_item_selected][y]->oAnimate))
            this->animateValue(false, this->scene_item_selected, y, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Y", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Y##101", &this->scene_item_settings[this->scene_item_selected][y]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][z]->oAnimate))
            this->animateValue(false, this->scene_item_selected, z, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Z", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Z##101", &this->scene_item_settings[this->scene_item_selected][z]->fValue, 0.0f, animateLimit);
    }
}

void GUI::addControlsSlider(std::string title, int idx, bool isGUI, float step, float limit, bool showAnimate, bool* animate, float* sliderValue) {
    if (title != "")
        ImGui::Text("%s", title.c_str());
    if (showAnimate) {
        std::string c_id = "##00" + std::to_string(idx);
        if (ImGui::Checkbox(c_id.c_str(), *(&animate)))
            this->animateValue(isGUI, (isGUI ? this->gui_item_selected : this->scene_item_selected), idx, step, limit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s", title.c_str());
        ImGui::SameLine();
    }
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), *(&sliderValue), 0.0, limit);
}

void GUI::addControlColor3(std::string title, glm::vec3* vValue, bool* bValue) {
    std::string ce_id = "##101" + title;
    std::string icon_id = ICON_MD_COLORIZE + ce_id;
    ImGui::TextColored(ImVec4((*(&vValue))->r, (*(&vValue))->g, (*(&vValue))->b, 255.0), "%s", title.c_str());
    ImGui::ColorEdit3(ce_id.c_str(), (float*)&(*vValue));
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 0));
    if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
        *bValue = !*bValue;
    ImGui::PopStyleColor(4);
    if (*bValue)
        this->colorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
}

void GUI::addControlColor4(std::string title, glm::vec4* vValue, bool* bValue) {
    std::string ce_id = "##101" + title;
    std::string icon_id = ICON_MD_COLORIZE + ce_id;
    ImGui::TextColored(ImVec4((*(&vValue))->r, (*(&vValue))->g, (*(&vValue))->b, (*(&vValue))->a), "%s", title.c_str());
    ImGui::ColorEdit4(ce_id.c_str(), (float*)&(*vValue), true);
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0, 0));
    if (ImGui::Button(icon_id.c_str(), ImVec2(0, 0)))
        *bValue = !*bValue;
    ImGui::PopStyleColor(4);
    if (*bValue)
        this->colorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
}

void GUI::resetValuesGUIControls() {
    this->so_GUI_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->sceneLights[0]->ambient = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1, 1, 1) });
    this->sceneLights[0]->diffuse = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1, 1, 1) });
    this->sceneLights[0]->specular = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 0.0, /*.color=*/ glm::vec3(1, 1, 1) });

    this->so_GUI_FOV = 45.0;
    this->so_GUI_ratio_w = 4.0f;
    this->so_GUI_ratio_h = 3.0f;
    this->so_GUI_plane_close = 0.1f;
    this->so_GUI_plane_far = 100.0f;
    this->so_GUI_grid_size = 10;
    this->so_outlineThickness = 1.01;

    for (int i=0; i<(int)this->gui_item_settings_default.size(); i++) {
        for (int j=0; j<(int)this->gui_item_settings[i].size(); j++) {
            this->gui_item_settings[i][j]->fValue = this->gui_item_settings_default[i][j]->fValue;
            this->gui_item_settings[i][j]->vValue = this->gui_item_settings_default[i][j]->vValue;
        }
    }
}

void GUI::contextModelRename() {
    ImGui::OpenPopup("Rename");

    ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Type the new model name:");
    ImGui::Text("(%s)", (*this->meshModelFaces)[this->scene_item_selected]->oFace.ModelTitle.c_str());

    if (this->guiModelRenameText[0] == '\0')
        strcpy(this->guiModelRenameText, (*this->meshModelFaces)[this->scene_item_selected]->oFace.ModelTitle.c_str());
    ImGui::InputText("", this->guiModelRenameText, sizeof(this->guiModelRenameText));

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*this->meshModelFaces)[this->scene_item_selected]->oFace.ModelTitle = std::string(this->guiModelRenameText);
        this->contextMenuRenameModelFunc(this->scene_item_selected, std::string(this->guiModelRenameText));
        ImGui::CloseCurrentPopup();
        this->cmenu_renameModel = false;
        this->guiModelRenameText[0] = '\0';
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) {
        ImGui::CloseCurrentPopup();
        this->cmenu_renameModel = false;
        this->guiModelRenameText[0] = '\0';
    }

    ImGui::EndPopup();
}

void GUI::contextModelDelete() {
    ImGui::OpenPopup("Delete?");

    ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Are you sure you want to delete this model?\n");
    ImGui::Text("(%s)", (*this->meshModelFaces)[this->scene_item_selected]->oFace.ModelTitle.c_str());

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        this->contextMenuDeleteModelFunc(this->scene_item_selected);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) { ImGui::CloseCurrentPopup(); this->cmenu_deleteYn = false; }

    ImGui::EndPopup();
}

void GUI::dialogSceneSettings() {
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", &this->displaySceneSettings, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->resetValuesSceneSettings();
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    const char* scene_items[this->meshModelFaces->size()];
    for (size_t i=0; i<this->meshModelFaces->size(); i++) {
        MeshModelFace *mmf = (*this->meshModelFaces)[i];
        scene_items[i] = mmf->oFace.ModelTitle.c_str();
    }

    // Scene Model
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::ListBox("", &this->scene_item_selected, scene_items, IM_ARRAYSIZE(scene_items));
    if (this->scene_item_selected > -1 && ImGui::BeginPopupContextItem("Actions")) {
        ImGui::MenuItem("Rename", NULL, &this->cmenu_renameModel);
        if (ImGui::MenuItem("Duplicate")) {
        }
        ImGui::MenuItem("Delete", NULL, &this->cmenu_deleteYn);
        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    if (this->cmenu_renameModel)
        this->contextModelRename();

    if (this->cmenu_deleteYn)
        this->contextModelDelete();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
    MeshModelFace *mmf = (*this->meshModelFaces)[this->scene_item_selected];
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "OBJ File:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.objFile.c_str());
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "Model:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.ModelTitle.c_str());
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "Material:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.materialID.c_str());
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "Vertices count:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.verticesCount);
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "Normals count:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.normalsCount);
    ImGui::TextColored(ImVec4(255, 0, 0, 255), "Indices count:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.indicesCount);

    ImGui::Separator();

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

    const char* tabsScene[] = {
        "\n" ICON_MD_TRANSFORM,
        "\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
        "\n" ICON_MD_3D_ROTATION,
        "\n" ICON_MD_OPEN_WITH,
        "\n" ICON_MD_TOLL,
        "\n" ICON_MD_FORMAT_PAINT,
        "\n" ICON_MD_LIGHTBULB_OUTLINE,
    };
    const char* tabsLabelsScene[] = { "General", "Scale", "Rotate", "Translate", "Displace", "Material", "Illumination" };
    const int numTabsScene = sizeof(tabsScene) / sizeof(tabsScene[0]);
    ImGui::TabLabels(numTabsScene, tabsScene, this->selectedTabScene, ImVec2(30.0, 30.0), tabsLabelsScene);
    ImGui::PopStyleColor(3);

    ImGui::Separator();

    switch (this->selectedTabScene) {
        case 0: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Properties");
            // cel shading
            ImGui::Checkbox("Cel Shading", &this->scene_item_settings[this->scene_item_selected][19]->bValue);
            // alpha
            ImGui::TextColored(ImVec4(1, 1, 1, this->scene_item_settings[this->scene_item_selected][20]->fValue), "Alpha Blending");
            ImGui::SliderFloat("", &this->scene_item_settings[this->scene_item_selected][20]->fValue, 0.0f, 1.0f);
            break;
        }
        case 1: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale Model");
            this->addControlsXYZ(false, 0, 1, 2, "scale", 0.01f, 1.0f);
            break;
        }
        case 2: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate model around axis");
            this->addControlsXYZ(false, 3, 4, 5, "rotation", 1.0f, 360.0f);
            break;
        }
        case 3: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move model by axis");
            this->addControlsXYZ(false, 6, 7, 8, "translation", 0.05f, this->so_GUI_grid_size);
            break;
        }
        case 4: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Displace model");
            this->addControlsXYZ(false, 9, 10, 11, "displacement", 0.05f, this->so_GUI_grid_size);
            break;
        }
        case 5: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Material of the model");
            this->addControlsSlider("Refraction", 12, false, 0.05f, 10.0f, true, &this->scene_item_settings[this->scene_item_selected][12]->oAnimate, &this->scene_item_settings[this->scene_item_selected][12]->fValue);
            this->addControlsSlider("Specular Exponent", 17, false, 10.0f, 1000.0f, true, &this->scene_item_settings[this->scene_item_selected][17]->oAnimate, &this->scene_item_settings[this->scene_item_selected][17]->fValue);
            this->addControlColor4("Ambient Color", &this->scene_item_settings[this->scene_item_selected][13]->vValue, &this->scene_item_settings[this->scene_item_selected][13]->bValue);
            this->addControlColor4("Diffuse Color", &this->scene_item_settings[this->scene_item_selected][14]->vValue, &this->scene_item_settings[this->scene_item_selected][14]->bValue);
            this->addControlColor4("Specular Color", &this->scene_item_settings[this->scene_item_selected][15]->vValue, &this->scene_item_settings[this->scene_item_selected][15]->bValue);
            this->addControlColor4("Emission Color", &this->scene_item_settings[this->scene_item_selected][16]->vValue, &this->scene_item_settings[this->scene_item_selected][16]->bValue);
            break;
        }
        case 6: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Illumination type");
            const char* illum_models_items[] = {
                "[0] Color on and Ambient off",  // "Shaderless" option in Blender, under "Shading" in Material tab
                "[1] Color on and Ambient on",
                "[2] Highlight on",
                "[3] Reflection on and Ray trace on",
                "[4] Transparency: Glass on\n    Reflection: Ray trace on",
                "[5] Reflection: Fresnel on\n    Ray trace on",
                "[6] Transparency: Refraction on\n    Reflection: Fresnel off\n    Ray trace on",
                "[7] Transparency: Refraction on\n    Reflection: Fresnel on\n    Ray trace on",
                "[8] Reflection on\n    Ray trace off",
                "[9] Transparency: Glass on\n    Reflection: Ray trace off",
                "[10] Casts shadows onto invisible surfaces"
            };
            ImGui::Combo("##987", &this->scene_item_settings[this->scene_item_selected][18]->iValue, illum_models_items, IM_ARRAYSIZE(illum_models_items));
            break;
        }
        default:
            break;
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void GUI::animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    std::thread animThread(&GUI::animateValueAsync, this, isGUI, elementID, sett_index, step, limit, doMinus);
    animThread.detach();
}

void GUI::animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    if (isGUI) {
        while (this->gui_item_settings[elementID][sett_index]->oAnimate) {
            if (this->isFrame) {
                float v = this->gui_item_settings[elementID][sett_index]->fValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                this->gui_item_settings[elementID][sett_index]->fValue = v;
                this->isFrame = false;
            }
        }
    }
    else {
        while (this->scene_item_settings[elementID][sett_index]->oAnimate) {
            if (this->isFrame) {
                float v = this->scene_item_settings[elementID][sett_index]->fValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                this->scene_item_settings[elementID][sett_index]->fValue = v;
                this->isFrame = false;
            }
        }
    }
}

void GUI::resetValuesSceneSettings() {
    for (int i=0; i<(int)this->scene_item_settings.size(); i++) {
        for (int j=0; j<(int)this->scene_item_settings[i].size(); j++) {
            this->scene_item_settings[i][j]->oAnimate = false;
            this->scene_item_settings[i][j]->iValue = this->scene_item_settings_default[i][j]->iValue;
            this->scene_item_settings[i][j]->fValue = this->scene_item_settings_default[i][j]->fValue;
            this->scene_item_settings[i][j]->bValue = this->scene_item_settings_default[i][j]->bValue;
            this->scene_item_settings[i][j]->vValue = this->scene_item_settings_default[i][j]->vValue;
        }
    }
}

void GUI::dialogSceneStats() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 10;
    int posY = windowHeight - 164;
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Stats", &this->displaySceneStats, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
    ImGui::Text("OpenGL version: %i.%i (%s)", Settings::Instance()->OpenGLMajorVersion, Settings::Instance()->OpenGLMinorVersion, glGetString(GL_VERSION));
    ImGui::Text("GLSL version: %i.%i (%s)", (Settings::Instance()->OpenGL_GLSL_Version / 100), (Settings::Instance()->OpenGL_GLSL_Version % 100), glGetString(GL_SHADING_LANGUAGE_VERSION));
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

#pragma mark - ImGui Implementation Render

void GUI::ImGui_Implementation_RenderDrawLists() {
    if (Settings::Instance()->OpenGLMajorVersion > 2)
        this->ImGui_SDL2GL32_Implementation_RenderDrawLists();
    else
        this->ImGui_SDL2GL21_Implementation_RenderDrawLists();
}

#pragma mark - ImGui Implementation SDL2, OpenGL 2.1

void GUI::ImGui_SDL2GL21_Implementation_RenderDrawLists() {
    ImDrawData* draw_data = ImGui::GetDrawData();

    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context

    ImGuiIO& io = ImGui::GetIO();
    float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    for (int n=0; n<draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
        const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

        for (int cmd_i=0; cmd_i<cmd_list->CmdBuffer.size(); cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
                pcmd->UserCallback(cmd_list, pcmd);
            else {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
#undef OFFSETOF

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

bool GUI::ImGui_SDL2GL21_Implementation_ProcessEvent(SDL_Event* event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
        case SDL_MOUSEWHEEL: {
            if (event->wheel.y > 0)
                this->gui_MouseWheel = 1;
            if (event->wheel.y < 0)
                this->gui_MouseWheel = -1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event->button.button == SDL_BUTTON_LEFT) this->gui_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT) this->gui_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE) this->gui_MousePressed[2] = true;
            return true;
        }
        case SDL_TEXTINPUT: {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
            io.KeysDown[key] = (event->type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            return true;
        }
    }
    return false;
}

bool GUI::ImGui_SDL2GL21_Implementation_CreateDeviceObjects() {
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    glGenTextures(1, &this->gui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, this->gui_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)this->gui_FontTexture;

    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();

    return true;
}

void GUI::ImGui_SDL2GL21_Implementation_InvalidateDeviceObjects() {
    if (this->gui_FontTexture) {
        glDeleteTextures(1, &this->gui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        this->gui_FontTexture = 0;
    }
}

bool GUI::ImGui_SDL2GL21_Implementation_Init() {
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(this->sdlWindow, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    return true;
}

void GUI::ImGui_SDL2GL21_Implementation_Shutdown() {
    this->ImGui_SDL2GL21_Implementation_InvalidateDeviceObjects();
    ImGui::Shutdown();
}

void GUI::ImGui_SDL2GL21_Implementation_NewFrame() {
    if (!this->gui_FontTexture)
        this->ImGui_SDL2GL21_Implementation_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    SDL_GetWindowSize(this->sdlWindow, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);

    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = this->gui_Time > 0.0 ? (float)(current_time - this->gui_Time) : (float)(1.0f/60.0f);
    this->gui_Time = current_time;

    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);
    else
        io.MousePos = ImVec2(-1,-1);

    io.MouseDown[0] = this->gui_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = this->gui_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = this->gui_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    this->gui_MousePressed[0] = this->gui_MousePressed[1] = this->gui_MousePressed[2] = false;

    io.MouseWheel = this->gui_MouseWheel;
    this->gui_MouseWheel = 0.0f;

    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

    ImGui::NewFrame();
}

#pragma mark - ImGui Implementation SDL2, OpenGL 3.2

void GUI::ImGui_SDL2GL32_Implementation_RenderDrawLists() {
    ImDrawData* draw_data = ImGui::GetDrawData();

    GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
    GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
    GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
    GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
    GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
    GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
    GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    ImGuiIO& io = ImGui::GetIO();
    float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    const float ortho_projection[4][4] =
    {
        { 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram(this->gui_ShaderHandle);
    glUniform1i(this->gui_AttribLocationTex, 0);
    glUniformMatrix4fv(this->gui_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(this->gui_VaoHandle);

    for (int n=0; n<draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, this->gui_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->gui_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBindVertexArray(last_vertex_array);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend)glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

bool GUI::ImGui_SDL2GL32_Implementation_Init() {
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDLK_a;
    io.KeyMap[ImGuiKey_C] = SDLK_c;
    io.KeyMap[ImGuiKey_V] = SDLK_v;
    io.KeyMap[ImGuiKey_X] = SDLK_x;
    io.KeyMap[ImGuiKey_Y] = SDLK_y;
    io.KeyMap[ImGuiKey_Z] = SDLK_z;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(this->sdlWindow, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#endif

    return true;
}

void GUI::ImGui_SDL2GL32_Implementation_Shutdown() {
    this->ImGui_SDL2GL32_Implementation_InvalidateDeviceObjects();
    ImGui::Shutdown();
}

void GUI::ImGui_SDL2GL32_Implementation_NewFrame() {
    if (!this->gui_FontTexture)
        this->ImGui_SDL2GL32_Implementation_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    SDL_GetWindowSize(this->sdlWindow, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = this->gui_Time > 0.0 ? (float)(current_time - this->gui_Time) : (float)(1.0f / 60.0f);
    this->gui_Time = current_time;

    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);
    else
        io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = this->gui_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = this->gui_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = this->gui_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    this->gui_MousePressed[0] = this->gui_MousePressed[1] = this->gui_MousePressed[2] = false;

    io.MouseWheel = this->gui_MouseWheel;
    this->gui_MouseWheel = 0.0f;

    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);
    ImGui::NewFrame();
}

bool GUI::ImGui_SDL2GL32_Implementation_ProcessEvent(SDL_Event* event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
        case SDL_MOUSEWHEEL: {
            if (event->wheel.y > 0)
                this->gui_MouseWheel = 1;
            if (event->wheel.y < 0)
                this->gui_MouseWheel = -1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event->button.button == SDL_BUTTON_LEFT)
                this->gui_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT)
                this->gui_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE)
                this->gui_MousePressed[2] = true;
            return true;
        }
        case SDL_TEXTINPUT: {
            ImGuiIO& io = ImGui::GetIO();
            io.AddInputCharactersUTF8(event->text.text);
            return true;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            int key = event->key.keysym.sym & ~SDLK_SCANCODE_MASK;
            io.KeysDown[key] = (event->type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            return true;
        }
    }
    return false;
}

void GUI::ImGui_SDL2GL32_Implementation_InvalidateDeviceObjects() {
    if (this->gui_VaoHandle)
        glDeleteVertexArrays(1, &this->gui_VaoHandle);
    if (this->gui_VboHandle)
        glDeleteBuffers(1, &this->gui_VboHandle);
    if (this->gui_ElementsHandle)
        glDeleteBuffers(1, &this->gui_ElementsHandle);
    this->gui_VaoHandle = this->gui_VboHandle = this->gui_ElementsHandle = 0;

    glDetachShader(this->gui_ShaderHandle, this->gui_VertHandle);
    glDeleteShader(this->gui_VertHandle);
    this->gui_VertHandle = 0;

    glDetachShader(this->gui_ShaderHandle, this->gui_FragHandle);
    glDeleteShader(this->gui_FragHandle);
    this->gui_FragHandle = 0;

    glDeleteProgram(this->gui_ShaderHandle);
    this->gui_ShaderHandle = 0;

    if (this->gui_FontTexture) {
        glDeleteTextures(1, &this->gui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        this->gui_FontTexture = 0;
    }
}

bool GUI::ImGui_SDL2GL32_Implementation_CreateDeviceObjects() {
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

    const GLchar *vertex_shader =
    "#version 330\n"
    "uniform mat4 ProjMtx;\n"
    "in vec2 Position;\n"
    "in vec2 UV;\n"
    "in vec4 Color;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = UV;\n"
    "	Frag_Color = Color;\n"
    "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
    "}\n";

    const GLchar* fragment_shader =
    "#version 330\n"
    "uniform sampler2D Texture;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "out vec4 Out_Color;\n"
    "void main()\n"
    "{\n"
    "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
    "}\n";

    this->gui_ShaderHandle = glCreateProgram();
    this->gui_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    this->gui_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->gui_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(this->gui_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(this->gui_VertHandle);
    glCompileShader(this->gui_FragHandle);
    glAttachShader(this->gui_ShaderHandle, this->gui_VertHandle);
    glAttachShader(this->gui_ShaderHandle, this->gui_FragHandle);
    glLinkProgram(this->gui_ShaderHandle);

    this->gui_AttribLocationTex = glGetUniformLocation(this->gui_ShaderHandle, "Texture");
    this->gui_AttribLocationProjMtx = glGetUniformLocation(this->gui_ShaderHandle, "ProjMtx");
    this->gui_AttribLocationPosition = glGetAttribLocation(this->gui_ShaderHandle, "Position");
    this->gui_AttribLocationUV = glGetAttribLocation(this->gui_ShaderHandle, "UV");
    this->gui_AttribLocationColor = glGetAttribLocation(this->gui_ShaderHandle, "Color");

    glGenBuffers(1, &this->gui_VboHandle);
    glGenBuffers(1, &this->gui_ElementsHandle);

    glGenVertexArrays(1, &this->gui_VaoHandle);
    glBindVertexArray(this->gui_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, this->gui_VboHandle);
    glEnableVertexAttribArray(this->gui_AttribLocationPosition);
    glEnableVertexAttribArray(this->gui_AttribLocationUV);
    glEnableVertexAttribArray(this->gui_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(this->gui_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(this->gui_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(this->gui_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    this->ImGui_SDL2GL32_Implementation_CreateFontsTexture();

    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindVertexArray(last_vertex_array);

    return true;
}

void GUI::ImGui_SDL2GL32_Implementation_CreateFontsTexture() {
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    glGenTextures(1, &this->gui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, this->gui_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)this->gui_FontTexture;

    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}
