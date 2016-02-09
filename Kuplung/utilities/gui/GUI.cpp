//
//  gui.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "utilities/gl/GLIncludes.h"
#include <SDL2/SDL_syswm.h>
#include <boost/algorithm/string/replace.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include "utilities/settings/Settings.h"
#include "GUI.hpp"
#include "utilities/gui/components/IconsFontAwesome.h"
#include "utilities/gui/components/IconsMaterialDesign.h"
#include "utilities/gui/components/Tabs.hpp"

GUI::~GUI() {
    this->destroy();
}

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
    this->needsFontChange = false;

    this->sceneLights = {};

    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;

    this->imguiImplementation = new SDL2OpenGL32();
    this->imguiImplementation->init(this->sdlWindow);

    this->componentLog = new Log();
    this->componentScreenshot = new Screenshot();

    this->componentFileBrowser = new FileBrowser();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&GUI::doLog, this, std::placeholders::_1), std::bind(&GUI::dialogFileBrowserProcessFile, this, std::placeholders::_1));

    this->componentColorPicker = new ColorPicker();

    this->componentFileEditor = new Editor();
    this->componentFileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100, std::bind(&GUI::doLog, this, std::placeholders::_1));

    this->windowStyle = new DialogStyle();
    this->windowStyle->init(std::bind(&GUI::doLog, this, std::placeholders::_1));
    ImGuiStyle& style = ImGui::GetStyle();
    this->windowStyle->saveDefault(style);
    style = this->windowStyle->loadCurrent();

    this->windowOptions = new DialogOptions();
    this->windowOptions->init(std::bind(&GUI::doLog, this, std::placeholders::_1));

    this->gui_item_selected = 0;
    this->scene_item_selected = -1;
    this->sceneLightsSelected = -1;
    this->selectedTabScene = 0;
    this->selectedTabGUICamera = 0;
    this->selectedTabGUIGrid = 0;
    this->selectedTabGUILight = 0;
    this->selectedTabGUITerrain = 0;

    this->isFrame = false;
    this->isProjection = true;
    this->fixedGridWorld = true;

    this->imguiImplementation->ImGui_Implementation_Init();

    this->windowOptions->loadFonts(&this->needsFontChange);
}

bool GUI::processEvent(SDL_Event *event) {
    return this->imguiImplementation->ImGui_Implementation_ProcessEvent(event);
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

    this->imguiImplementation->ImGui_Implementation_Shutdown();
}

void GUI::doLog(std::string logMessage) {
    printf("%s\n", logMessage.c_str());
    this->componentLog->addToLog("%s\n", logMessage.c_str());
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

void GUI::addSceneLight(std::string lightTitle, GUILightType type) {
    GUILightObject *glo_ambient = new GUILightObject();
    glo_ambient->colorPickerOpen = false;
    glo_ambient->color = glm::vec3(1, 1, 1);
    glo_ambient->strength = 0.3;
    glo_ambient->doAnimation = false;

    GUILightObject *glo_diffuse = new GUILightObject();
    glo_diffuse->colorPickerOpen = false;
    switch (type) {
        case GUILightType_Point:
            glo_diffuse->color = glm::vec3(1, 1, 1);
            break;
        case GUILightType_Sun:
            glo_diffuse->color = glm::vec3(0, 0, 0);
            break;
        default:
            break;
    }
    glo_diffuse->strength = 1.0;
    glo_diffuse->doAnimation = false;

    GUILightObject *glo_specular = new GUILightObject();
    glo_specular->colorPickerOpen = false;
    glo_specular->color = glm::vec3(1, 1, 1);
    glo_specular->strength = 0.0;
    glo_specular->doAnimation = false;

    GUISceneLight *gsl = new GUISceneLight();
    gsl->lightTitle = lightTitle;
    gsl->lightType = type;
    gsl->ambient = glo_ambient;
    gsl->diffuse = glo_diffuse;
    gsl->specular = glo_specular;

    this->sceneLights.push_back(gsl);
}

void GUI::hideSceneSettings() {
    this->displaySceneControls = false;
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

void GUI::renderStart(bool isFrame) {
    this->isFrame = isFrame;

    this->imguiImplementation->ImGui_Implementation_NewFrame();

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
            ImGui::Separator();
            if (ImGui::BeginMenu(ICON_FA_LIGHTBULB_O " Add Light")) {
                if (ImGui::MenuItem(ICON_FA_LIGHTBULB_O " Point"))
                    this->addSceneLight("Point Light " + std::to_string((int)this->sceneLights.size()), GUILightType_Point);
                if (ImGui::MenuItem(ICON_FA_SUN_O " Sun"))
                    this->addSceneLight("Sun Light " + std::to_string((int)this->sceneLights.size()), GUILightType_Sun);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem(Settings::Instance()->showGrid ? ICON_FA_TOGGLE_ON " Hide Grid" : ICON_FA_TOGGLE_OFF " Show Grid", NULL, &Settings::Instance()->showGrid);
            ImGui::MenuItem(Settings::Instance()->showLight ? ICON_FA_TOGGLE_ON " Hide Light" : ICON_FA_TOGGLE_OFF " Show Light", NULL, &Settings::Instance()->showLight);
            ImGui::MenuItem(Settings::Instance()->showAxes ? ICON_FA_TOGGLE_ON " Hide Axes" : ICON_FA_TOGGLE_OFF " Show Axes", NULL, &Settings::Instance()->showAxes);
            ImGui::Separator();
            ImGui::MenuItem(this->displayGUIControls ? ICON_FA_TOGGLE_ON " GUI Controls" : ICON_FA_TOGGLE_OFF " GUI Controls", NULL, &this->displayGUIControls);
            ImGui::MenuItem(this->displaySceneControls ? ICON_FA_TOGGLE_ON " Scene Controls" : ICON_FA_TOGGLE_OFF " Scene Controls", NULL, &this->displaySceneControls);
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

    if (this->displaySceneControls)
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
    if (this->needsFontChange) {
        this->windowOptions->loadFonts(&this->needsFontChange);
        this->imguiImplementation->ImGui_Implementation_CreateFontsTexture();
    }

    ImGui::Render();
    this->imguiImplementation->ImGui_Implementation_RenderDrawLists();
}

#pragma mark - GUI controls

void GUI::contextLightDelete() {
    ImGui::OpenPopup("Delete?");

    ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Are you sure you want to delete this model?\n");
    ImGui::Text("(%s)", (*this->meshModelFaces)[this->scene_item_selected]->oFace.ModelTitle.c_str());

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*this->meshModelFaces).erase((*this->meshModelFaces).begin() + this->scene_item_selected);
        this->removeSceneModelSettings(this->scene_item_selected);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) { ImGui::CloseCurrentPopup(); this->cmenu_deleteYn = false; }

    ImGui::EndPopup();
}

void GUI::dialogGUIControls() {
//    ImGui::Begin("Splitter test");

//    static float w = 200.0f;
//    static float h = 300.0f;
//    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
//    ImGui::BeginChild("child1", ImVec2(w, h), true);
//    ImGui::EndChild();
//    ImGui::SameLine();
//    ImGui::InvisibleButton("vsplitter", ImVec2(8.0f,h));
//    if (ImGui::IsItemActive())
//        w += ImGui::GetIO().MouseDelta.x;
//    ImGui::SameLine();
//    ImGui::BeginChild("child2", ImVec2(0, h), true);
//    ImGui::EndChild();
//    ImGui::InvisibleButton("hsplitter", ImVec2(-1,8.0f));
//    if (ImGui::IsItemActive())
//        h += ImGui::GetIO().MouseDelta.y;
//    ImGui::BeginChild("child3", ImVec2(0,0), true);
//    ImGui::EndChild();
//    ImGui::PopStyleVar();

//    ImGui::End();



    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("GUI Controls", &this->displayGUIControls, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->resetValuesGUIControls();
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(255, 0, 0));
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::BeginChild("Global Items", ImVec2(0, 130), true);
    for (int i=0; i<4; i++) {
        switch (i) {
            case 0: {
                ImGui::Indent();
                if (ImGui::Selectable(ICON_FA_ASTERISK " General", this->gui_item_selected == i)) {
                    this->gui_item_selected = i;
                    this->sceneLightsSelected = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 1: {
                ImGui::Indent();
                if (ImGui::Selectable(ICON_FA_VIDEO_CAMERA " Camera", this->gui_item_selected == i)) {
                    this->gui_item_selected = i;
                    this->sceneLightsSelected = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 2: {
                ImGui::Indent();
                if (ImGui::Selectable(ICON_FA_BARS " Grid", this->gui_item_selected == i)) {
                    this->gui_item_selected = i;
                    this->sceneLightsSelected = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 3: {
                if (ImGui::TreeNode(ICON_FA_LIGHTBULB_O " Lights")) {
                    for (int j = 0; j<(int)this->sceneLights.size(); j++) {
                        std::string lt = this->sceneLights[j]->lightTitle;
                        ImGui::Bullet();
                        if (ImGui::Selectable(lt.c_str(), this->sceneLightsSelected == j)) {
                            this->sceneLightsSelected = j;
                            this->gui_item_selected = 3;
                        }
                    }
                    ImGui::TreePop();
                }
                break;
            }
            case 4: {
                ImGui::Indent();
                if (ImGui::Selectable("Terrain", this->gui_item_selected == i)) {
                    this->gui_item_selected = i;
                    this->sceneLightsSelected = -1;
                }
                ImGui::Unindent();
                break;
            }
            default:
                break;
        }
    }
    ImGui::EndChild();
    ImGui::PopItemWidth();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

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

                    GUISceneLight *light = this->sceneLights[this->sceneLightsSelected];

                    this->addControlColor3("Ambient Color", &light->ambient->color, &light->ambient->colorPickerOpen);
                    this->addControlsSliderLights("Ambient Strength", 1, light->ambient);

                    this->addControlColor3("Diffuse Color", &light->diffuse->color, &light->diffuse->colorPickerOpen);
                    this->addControlsSliderLights("Diffuse Strength", 2, light->diffuse);

                    this->addControlColor3("Specular Color", &light->specular->color, &light->specular->colorPickerOpen);
                    this->addControlsSliderLights("Specular Strength", 3, light->specular);
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
        default:
            break;
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void GUI::resetValuesGUIControls() {
    this->so_GUI_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    this->sceneLights.clear();
    this->addSceneLight("Point Light", GUILightType_Point);

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

#pragma mark - Models context menu

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
        (*this->meshModelFaces).erase((*this->meshModelFaces).begin() + this->scene_item_selected);
        this->removeSceneModelSettings(this->scene_item_selected);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) { ImGui::CloseCurrentPopup(); this->cmenu_deleteYn = false; }

    ImGui::EndPopup();
}

#pragma mark - Scene controls

void GUI::dialogSceneSettings() {
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", &this->displaySceneControls, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->resetValuesSceneControls();
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    const char* scene_items[this->meshModelFaces->size()];
    for (size_t i=0; i<this->meshModelFaces->size(); i++) {
        std::string t = std::string(ICON_FA_CUBE) + " " + (*this->meshModelFaces)[i]->oFace.ModelTitle;
        // ICON_FA_CUBE
        scene_items[i] = (*this->meshModelFaces)[i]->oFace.ModelTitle.c_str();
    }

    // Scene Model
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::ListBox("", &this->scene_item_selected, scene_items, IM_ARRAYSIZE(scene_items));
    ImGui::PopStyleVar();
    if (this->scene_item_selected > -1 && ImGui::BeginPopupContextItem("Actions")) {
        ImGui::MenuItem("Rename", NULL, &this->cmenu_renameModel);
        if (ImGui::MenuItem("Duplicate")) {
            MeshModelFace *mmf = (*this->meshModelFaces)[this->scene_item_selected]->clone((int)(*this->meshModelFaces).size() + 1);
            (*this->meshModelFaces).push_back(mmf);
            this->addSceneModelSettings();

            int i = (int)this->scene_item_settings.size() - 1;
            this->setModelSetting((int)i, 12, 1, mmf->oFace.faceMaterial.opticalDensity);
            this->setModelSetting((int)i, 17, 1, mmf->oFace.faceMaterial.specularExp);
            this->setModelSetting((int)i, 18, 1, mmf->oFace.faceMaterial.illumination);
            this->setModelSetting((int)i, 13, 1, 1, false, glm::vec4(mmf->oFace.faceMaterial.ambient.r, mmf->oFace.faceMaterial.ambient.g, mmf->oFace.faceMaterial.ambient.b, mmf->oFace.faceMaterial.ambient.a));
            this->setModelSetting((int)i, 14, 1, 1, false, glm::vec4(mmf->oFace.faceMaterial.diffuse.r, mmf->oFace.faceMaterial.diffuse.g, mmf->oFace.faceMaterial.diffuse.b, mmf->oFace.faceMaterial.diffuse.a));
            this->setModelSetting((int)i, 15, 1, 1, false, glm::vec4(mmf->oFace.faceMaterial.specular.r, mmf->oFace.faceMaterial.specular.g, mmf->oFace.faceMaterial.specular.b, mmf->oFace.faceMaterial.specular.a));
            this->setModelSetting((int)i, 16, 1, 1, false, glm::vec4(mmf->oFace.faceMaterial.emission.r, mmf->oFace.faceMaterial.emission.g, mmf->oFace.faceMaterial.emission.b, mmf->oFace.faceMaterial.emission.a));
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

    if ((int)this->scene_item_settings.size() > this->scene_item_selected) {
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
    }
    ImGui::PopItemWidth();

    ImGui::End();
}

void GUI::resetValuesSceneControls() {
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

#pragma mark - Simple dialogs

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

void GUI::dialogFileBrowser() {
    this->componentFileBrowser->setStyleBrowser(false);
    this->componentFileBrowser->draw("File Browser", &this->showFileDialog);
}

void GUI::dialogStyleBrowser() {
    this->componentFileBrowser->setStyleBrowser(true);
    this->componentFileBrowser->draw("Open Style", &this->showStyleDialog);
}

void GUI::dialogFileBrowserProcessFile(FBEntity file) {
    if (this->showStyleDialog)
        this->windowStyle->load(file.path);
    this->processFile(file);
    this->showFileDialog = false;
    this->showStyleDialog = false;
}

void GUI::dialogScreenshot() {
    this->componentScreenshot->ShowScreenshotsWindow(&this->showScreenshotWindow);
}

void GUI::dialogEditor() {
    this->componentFileEditor->draw(std::bind(&GUI::fileEditorSaved, this, std::placeholders::_1), "Editor", &this->showEditor);
}

void GUI::fileEditorSaved(std::string fileName) {
    this->doFileShaderCompile(fileName);
}

void GUI::dialogLog() {
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = windowWidth - Settings::Instance()->frameLog_Width - 10;
    int posY = windowHeight - Settings::Instance()->frameLog_Height - 10;
    this->componentLog->init(posX, posY, Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height);
    this->componentLog->draw("Log Window");
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
    this->windowOptions->showOptionsWindow(ref, this->windowStyle, &this->showOptions, &this->needsFontChange);
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

#pragma mark - ImGui Helpers

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
            this->animateValue(isGUI, (isGUI ? 0 : 1), idx, step, limit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s", title.c_str());
        ImGui::SameLine();
    }
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), *(&sliderValue), 0.0, limit);
}

void GUI::addControlsSliderLights(std::string title, int idx, GUILightObject* light) {
    if (title != "")
        ImGui::Text("%s", title.c_str());
    std::string c_id = "##00" + std::to_string(idx);
    if (ImGui::Checkbox(c_id.c_str(), &light->doAnimation))
        this->animateValueLights(light);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Animate %s", title.c_str());
    ImGui::SameLine();
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), &light->strength, 0.0, 1.0);
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
        this->componentColorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
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
        this->componentColorPicker->show(title.c_str(), &(*bValue), (float*)&(*vValue), true);
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

void GUI::animateValueLights(GUILightObject* light) {
    std::thread animThreadLight(&GUI::animateValueAsyncLights, this, light);
    animThreadLight.detach();
}

void GUI::animateValueAsyncLights(GUILightObject* light) {
    while (light->doAnimation) {
        if (this->isFrame) {
            float v = light->strength;
            v += 0.05;
            if (v > 1.0)
                v = -1.0;
            light->strength = v;
            this->isFrame = false;
        }
    }
}
