//
//  gui.cpp
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "utilities/gl/GLIncludes.h"
#include <SDL2/SDL_syswm.h>
#include <string>
#include <sstream>
#include "utilities/settings/Settings.h"
#include "GUI.hpp"

static double gui_Time = 0.0f;
static bool gui_MousePressed[3] = { false, false, false };
static float gui_MouseWheel = 0.0f;
static GLuint gui_FontTexture = 0;
static int gui_ShaderHandle = 0, gui_VertHandle = 0, gui_FragHandle = 0;
static int gui_AttribLocationTex = 0, gui_AttribLocationProjMtx = 0;
static int gui_AttribLocationPosition = 0, gui_AttribLocationUV = 0, gui_AttribLocationColor = 0;
static unsigned int gui_VboHandle = 0, gui_VaoHandle = 0, gui_ElementsHandle = 0;
ImVec4 clear_color = ImColor(114, 144, 154);

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
    this->showAppMetrics = false;
    this->showAboutKuplung = false;
    this->showAboutImgui = false;
    this->displaySceneStats = false;
    this->showHeightmap = false;
    this->showEditor = false;
    this->newHeightmap = false;

    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;

    this->windowLog = new GUILog();
    this->windowScreenshot = new GUIScreenshot();
    this->windowFileBrowser = new GUIFileBrowser();

    this->fileEditor = new GUIEditor();
    this->fileEditor->init(Settings::Instance()->appFolder(), posX, posY, 100, 100, std::bind(&GUI::doLog, this, std::placeholders::_1));

    this->gui_item_selected = 0;

    this->isFrame = false;
    this->isProjection = true;
    this->fixedGridWorld = true;

    if (Settings::Instance()->OpenGLMajorVersion > 2)
        this->ImGui_SDL2GL32_Implementation_Init();
    else
        this->ImGui_SDL2GL21_Implementation_Init();
}

bool GUI::processEvent(SDL_Event *event) {
    if (Settings::Instance()->OpenGLMajorVersion > 2)
        return this->ImGui_SDL2GL32_Implementation_ProcessEvent(event);
    else
        return this->ImGui_SDL2GL21_Implementation_ProcessEvent(event);
}

void GUI::destroy() {
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

void GUI::initGUIControls(int guiObjectsCount, std::map<int, std::vector<float>> initialSettings) {
    for (int i=0; i<guiObjectsCount; i++) {
        std::vector<GUIObjectSetting> setts;
        for (int j=0; j<21; j++) {
            GUIObjectSetting gos;
            gos.oIndex = j;
            gos.oAnimate = false;
            gos.oValue = initialSettings[i][j];
            setts.push_back(gos);
        }
        this->gui_item_settings[i] = setts;
        this->gui_item_settings_default[i] = setts;
    }
}

void GUI::showGUIControls() {
    this->displayGUIControls = true;
}

void GUI::hideGUIControls() {
    this->displayGUIControls = true;
}

void GUI::showSceneSettings(std::map<int, std::string> scene_models) {
    this->scene_item_selected = 0;
    for (int i=0; i<(int)scene_models.size(); i++) {
        std::vector<GUIObjectSetting> setts = {};

        GUIObjectSetting gos;
        gos.oIndex = 0;
        gos.oAnimate = false;

        // scale
        gos.oIndex += 1; gos.oValue = 1.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 1.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 1.0; setts.push_back(gos);

        // rotate
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);

        // translate
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);

        // displacement
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.0; setts.push_back(gos);

        // refraction
        gos.oIndex += 1; gos.oValue = 1.0; setts.push_back(gos);
        // shininess
        gos.oIndex += 1; gos.oValue = 1.0; setts.push_back(gos);

        // material
        gos.oIndex += 1; gos.oValue = 0.5; gos.vValue = glm::vec3(1, 1, 1); setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 0.5; gos.vValue = glm::vec3(1, 1, 1); setts.push_back(gos);
        gos.oIndex += 1; gos.oValue = 1.0; gos.vValue = glm::vec3(1, 1, 1); setts.push_back(gos);

        this->scene_item_settings[i] = setts;
        this->scene_item_settings_default[i] = setts;
    }
    this->sceneModels = scene_models;
    this->displaySceneSettings = true;
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

void GUI::setShaderEditor(std::function<void(std::string, std::string)> fileShaderCompile) {
    this->doFileShaderCompile = fileShaderCompile;
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
            if (ImGui::MenuItem("New"))
                this->newScene();
            ImGui::MenuItem("Open", NULL, &showFileDialog);

            if (ImGui::BeginMenu("Open Recent")) {
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

            ImGui::MenuItem("Editor", NULL, &this->showEditor);
            ImGui::MenuItem("Screenshot", NULL, &this->showScreenshotWindow);

            if (ImGui::BeginMenu("Settings")) {
                ImGui::MenuItem("Show Log Window", NULL, &Settings::Instance()->logDebugInfo);
                ImGui::Separator();
                ImGui::MenuItem("Options", NULL, &this->showOptions);
                ImGui::EndMenu();
            }

            ImGui::Separator();
#ifdef _WIN32
            if (ImGui::MenuItem("Quit", "Alt+F4"))
#else
            if (ImGui::MenuItem("Quit", "Cmd+Q"))
#endif
                this->quitApp();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene")) {
            if (this->isProjection)
                ImGui::MenuItem("Projection perspective", NULL, &this->isProjection);
            else
                ImGui::MenuItem("Orthographic perspective", NULL, &this->isProjection);

            if (Settings::Instance()->wireframesMode)
                ImGui::MenuItem("Wireframes mode", NULL, &Settings::Instance()->wireframesMode);
            else
                ImGui::MenuItem("Fill mode", NULL, &Settings::Instance()->wireframesMode);

            if (Settings::Instance()->showGrid)
                ImGui::MenuItem("Hide Grid", NULL, &Settings::Instance()->showGrid);
            else
                ImGui::MenuItem("Show Grid", NULL, &Settings::Instance()->showGrid);

            if (Settings::Instance()->showLight)
                ImGui::MenuItem("Hide Light", NULL, &Settings::Instance()->showLight);
            else
                ImGui::MenuItem("Show Light", NULL, &Settings::Instance()->showLight);

            if (Settings::Instance()->showAxes)
                ImGui::MenuItem("Hide Axes", NULL, &Settings::Instance()->showAxes);
            else
                ImGui::MenuItem("Show Axes", NULL, &Settings::Instance()->showAxes);
            ImGui::Separator();
            ImGui::MenuItem("GUI Controls", NULL, &this->displayGUIControls);
            ImGui::MenuItem("Scene Settings", NULL, &this->displaySceneSettings);
            ImGui::MenuItem("Scene Statistics", NULL, &this->displaySceneStats);
            ImGui::MenuItem("Display Terrain", NULL, &this->showHeightmap);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("Metrics", NULL, &this->showAppMetrics);
            ImGui::MenuItem("About ImGui", NULL, &this->showAboutImgui);
            ImGui::MenuItem("About Kuplung", NULL, &this->showAboutKuplung);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo Window", NULL, &this->showDemoWindow);
            ImGui::EndMenu();
        }

        ImGui::Text("  --> %.1f FPS | %d vertices, %d indices (%d triangles)", ImGui::GetIO().Framerate, ImGui::GetIO().MetricsRenderVertices, ImGui::GetIO().MetricsRenderIndices, ImGui::GetIO().MetricsRenderIndices / 3);

        ImGui::EndMainMenuBar();
    }

    if (this->showFileDialog)
        this->dialogFileBrowser();

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
        this->dialogOptions();

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
        ImGui::Text("\n\n");
        ImGui::Text("    Processing ... \n");
        if (this->loadingPercentage > 0.0)
            ImGui::Text("       %0.2f%%         \n", this->loadingPercentage);
        ImGui::Text("\n\n");
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
    int windowWidth, windowHeight;
    SDL_GetWindowSize(this->sdlWindow, &windowWidth, &windowHeight);
    int posX = 50, posY = 50;
    this->windowFileBrowser->init(Settings::Instance()->logFileBrowser, posX, posY, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&GUI::doLog, this, std::placeholders::_1), std::bind(&GUI::dialogFileBrowserProcessFile, this, std::placeholders::_1));
    this->windowFileBrowser->draw("File Browser", &this->showFileDialog);
}

void GUI::dialogFileBrowserProcessFile(FBEntity file) {
    this->showFileDialog = false;
    this->processFile(file);
}

void GUI::dialogScreenshot() {
    this->windowScreenshot->ShowScreenshotsWindow(&this->showScreenshotWindow);
}

void GUI::dialogEditor() {
    this->fileEditor->draw(std::bind(&GUI::fileEditorSaved, this, std::placeholders::_1, std::placeholders::_2), "Editor", &this->showEditor);
}

void GUI::fileEditorSaved(std::string fileName, std::string fileSource) {
    this->doFileShaderCompile(fileName, fileSource);
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

void GUI::dialogOptions() {
    ImGui::Begin("Options", &this->showOptions, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders);
    if (ImGui::Checkbox("Log Messages", &Settings::Instance()->logDebugInfo))
        Settings::Instance()->saveSettings();
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->heightmapWidth, this->heightmapHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
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
    if (ImGui::Button("Reset GUI Control Values"))
        this->resetValuesGUIControls();
    ImGui::PopStyleColor(3);

    if (ImGui::CollapsingHeader("General")) {
        ImGui::Text("Field of view");
        ImGui::SliderFloat("FOV", &this->so_GUI_FOV, 0.0f, 180.0f);
        ImGui::Separator();
        ImGui::Text("Ratio");
        ImGui::SliderFloat("W", &this->so_GUI_ratio_w, 0.0f, 5.0f);
        ImGui::SliderFloat("H", &this->so_GUI_ratio_h, 0.0f, 5.0f);
        ImGui::Separator();
        ImGui::Text("Planes");
        ImGui::SliderFloat("Close", &this->so_GUI_plane_close, 0.0f, 1.0f);
        ImGui::SliderFloat("Far", &this->so_GUI_plane_far, 0.0f, 100.0f);
        ImGui::Separator();
        ImGui::Text("Grid size");
        ImGui::SliderInt("Squares", &this->so_GUI_grid_size, 0, 100);
        ImGui::Separator();
        ImGui::Checkbox("Grid fixed with World", &this->fixedGridWorld);
        ImGui::Separator();
        ImGui::TextColored(ImVec4(this->so_GUI_outlineColor.r, this->so_GUI_outlineColor.g, this->so_GUI_outlineColor.b, 1.0), "Outline color");
        ImGui::SliderFloat("Red", &this->so_GUI_outlineColor.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green", &this->so_GUI_outlineColor.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue", &this->so_GUI_outlineColor.b, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::Text("Outline thickness");
        ImGui::SliderFloat("Thickness", &this->so_outlineThickness, 1.01f, 2.0f);
    }
    ImGui::Separator();

    const char* gui_items[] = { "Camera", "Grid", "Light", "Terrain" };
    ImGui::Combo("GUI Item", &this->gui_item_selected, gui_items, IM_ARRAYSIZE(gui_items));
    if (this->gui_item_selected == 0 && ImGui::TreeNode("LookAt")) {
        ImGui::SliderFloat("Eye X", &this->gui_item_settings[this->gui_item_selected][0].oValue, -10.0f, 10.0f);
        ImGui::SliderFloat("Eye Y", &this->gui_item_settings[this->gui_item_selected][1].oValue, -100.0f, 10.0f);
        ImGui::SliderFloat("Eye Z", &this->gui_item_settings[this->gui_item_selected][2].oValue, 0.0f, 90.0f);
        ImGui::Separator();
        ImGui::SliderFloat("Center X", &this->gui_item_settings[this->gui_item_selected][3].oValue, -10.0f, 10.0f);
        ImGui::SliderFloat("Center Y", &this->gui_item_settings[this->gui_item_selected][4].oValue, -10.0f, 10.0f);
        ImGui::SliderFloat("Center Z", &this->gui_item_settings[this->gui_item_selected][5].oValue, 0.0f, 45.0f);
        ImGui::Separator();
        ImGui::SliderFloat("Up X", &this->gui_item_settings[this->gui_item_selected][6].oValue, -10.0f, 10.0f);
        ImGui::SliderFloat("Up Y", &this->gui_item_settings[this->gui_item_selected][7].oValue, -1.0f, 1.0f);
        ImGui::SliderFloat("Up Z", &this->gui_item_settings[this->gui_item_selected][8].oValue, -10.0f, 10.0f);
        ImGui::TreePop();
    }

    if (this->gui_item_selected > 0 && ImGui::TreeNode("Scale")) {
        ImGui::SliderFloat("X##1", &this->gui_item_settings[this->gui_item_selected][9].oValue, 0.0f, 1.0f);
        ImGui::SliderFloat("Y##1", &this->gui_item_settings[this->gui_item_selected][10].oValue, 0.0f, 1.0f);
        ImGui::SliderFloat("Z##1", &this->gui_item_settings[this->gui_item_selected][11].oValue, 0.0f, 1.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Rotate")) {
        if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][12].oAnimate))
            this->animateValue(true, this->gui_item_selected, 12, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##2", &this->gui_item_settings[this->gui_item_selected][12].oValue, 0.0f, 360.0f);
        if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][13].oAnimate))
            this->animateValue(true, this->gui_item_selected, 13, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##2", &this->gui_item_settings[this->gui_item_selected][13].oValue, 0.0f, 360.0f);
        if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][14].oAnimate))
            this->animateValue(true, this->gui_item_selected, 14, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##2", &this->gui_item_settings[this->gui_item_selected][14].oValue, 0.0f, 360.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Translate")) {
        if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][15].oAnimate))
            this->animateValue(true, this->gui_item_selected, 15, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##3", &this->gui_item_settings[this->gui_item_selected][15].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
        if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][16].oAnimate))
            this->animateValue(true, this->gui_item_selected, 16, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##3", &this->gui_item_settings[this->gui_item_selected][16].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
        if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][17].oAnimate))
            this->animateValue(true, this->gui_item_selected, 17, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##3", &this->gui_item_settings[this->gui_item_selected][17].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);
        ImGui::TreePop();
    }

    if (this->gui_item_selected == 2 && ImGui::TreeNode("Light")) {
        ImGui::TextColored(ImVec4(this->so_GUI_lightAmbient.r, this->so_GUI_lightAmbient.g, this->so_GUI_lightAmbient.b, 1.0), "Ambient");
        ImGui::SliderFloat("Red##001", &this->so_GUI_lightAmbient.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##002", &this->so_GUI_lightAmbient.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##003", &this->so_GUI_lightAmbient.b, 0.0f, 1.0f);

        if (ImGui::Checkbox("##2", &this->gui_item_settings[this->gui_item_selected][18].oAnimate))
            this->animateValue(true, this->gui_item_selected, 18, 0.1f, 4.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate ambient strength");
        ImGui::SameLine(); ImGui::SliderFloat("Strength##01", &this->gui_item_settings[this->gui_item_selected][18].oValue, 0.0, 4.0);

        ImGui::TextColored(ImVec4(this->so_GUI_lightDiffuse.r, this->so_GUI_lightDiffuse.g, this->so_GUI_lightDiffuse.b, 1.0), "Diffuse");
        ImGui::SliderFloat("Red##004", &this->so_GUI_lightDiffuse.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##005", &this->so_GUI_lightDiffuse.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##006", &this->so_GUI_lightDiffuse.b, 0.0f, 1.0f);

        if (ImGui::Checkbox("##3", &this->gui_item_settings[this->gui_item_selected][19].oAnimate))
            this->animateValue(true, this->gui_item_selected, 19, 0.1f, 6.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate diffuse strength");
        ImGui::SameLine(); ImGui::SliderFloat("Strength##02", &this->gui_item_settings[this->gui_item_selected][19].oValue, 0.0, 6.0);

        ImGui::TextColored(ImVec4(this->so_GUI_lightSpecular.r, this->so_GUI_lightSpecular.g, this->so_GUI_lightSpecular.b, 1.0), "Specular");
        ImGui::SliderFloat("Red##007", &this->so_GUI_lightSpecular.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##008", &this->so_GUI_lightSpecular.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##009", &this->so_GUI_lightSpecular.b, 0.0f, 1.0f);

        if (ImGui::Checkbox("##1", &this->gui_item_settings[this->gui_item_selected][20].oAnimate))
            this->animateValue(true, this->gui_item_selected, 20, 0.1f, 4.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate specular strength");
        ImGui::SameLine(); ImGui::SliderFloat("Strength##03", &this->gui_item_settings[this->gui_item_selected][20].oValue, 0.0, 4.0);

        ImGui::TreePop();
    }

    ImGui::End();
}

void GUI::resetValuesGUIControls() {
    this->so_GUI_outlineColor = glm::vec3(1.0, 0.0, 0.0);
    this->so_GUI_lightAmbient = glm::vec3(1.0, 1.0, 1.0);
    this->so_GUI_lightDiffuse = glm::vec3(1.0, 1.0, 1.0);
    this->so_GUI_lightSpecular = glm::vec3(1.0, 1.0, 1.0);

    this->so_GUI_FOV = 45.0;
    this->so_GUI_ratio_w = 4.0f;
    this->so_GUI_ratio_h = 3.0f;
    this->so_GUI_plane_close = 0.1f;
    this->so_GUI_plane_far = 100.0f;
    this->so_GUI_grid_size = 10;
    this->so_outlineThickness = 1.01;

    for (int i=0; i<(int)this->gui_item_settings_default.size(); i++) {
        for (int j=0; j<(int)this->gui_item_settings[i].size(); j++) {
            this->gui_item_settings[i][j] = this->gui_item_settings_default[i][j];
        }
    }
}

void GUI::dialogSceneSettings() {
    ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", &this->displaySceneSettings, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset Scene Settings Values"))
        this->resetValuesSceneSettings();
    ImGui::PopStyleColor(3);

    ImGui::TextColored(ImVec4(1, 1, 1, this->so_Alpha), "Alpha Blending");
    ImGui::SliderFloat("", &this->so_Alpha, 0.0f, 1.0f); // Alpha
    ImGui::Separator();

    int modelsCount = (int)this->sceneModels.size();
    const char* scene_items[modelsCount];
    for (int i=0; i<(int)this->sceneModels.size(); i++) {
        scene_items[i] = this->sceneModels[i].c_str();
    }

    ImGui::Combo("", &this->scene_item_selected, scene_items, IM_ARRAYSIZE(scene_items)); // Scene Model

    if (ImGui::TreeNode("Scale")) {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][0].oAnimate))
            this->animateValue(false, this->scene_item_selected, 0, 0.01f, 1.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate scale by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##101", &this->scene_item_settings[this->scene_item_selected][0].oValue, 0.0f, 1.0f);

        if (ImGui::Checkbox("##2", &this->scene_item_settings[this->scene_item_selected][1].oAnimate))
            this->animateValue(false, this->scene_item_selected, 1, 0.01f, 1.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate scale by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##101", &this->scene_item_settings[this->scene_item_selected][1].oValue, 0.0f, 1.0f);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][2].oAnimate))
            this->animateValue(false, this->scene_item_selected, 2, 0.01f, 1.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate scale by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##101", &this->scene_item_settings[this->scene_item_selected][2].oValue, 0.0f, 1.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Rotate")) {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][3].oAnimate))
            this->animateValue(false, this->scene_item_selected, 3, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##102", &this->scene_item_settings[this->scene_item_selected][3].oValue, 0.0f, 360.0f);

        if (ImGui::Checkbox("##2", &this->scene_item_settings[this->scene_item_selected][4].oAnimate))
            this->animateValue(false, this->scene_item_selected, 4, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##102", &this->scene_item_settings[this->scene_item_selected][4].oValue, 0.0f, 360.0f);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][5].oAnimate))
            this->animateValue(false, this->scene_item_selected, 5, 1.0f, 360.0, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate rotation by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##102", &this->scene_item_settings[this->scene_item_selected][5].oValue, 0.0f, 360.0f);
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Translate")) {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][6].oAnimate))
            this->animateValue(false, this->scene_item_selected, 6, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##103", &this->scene_item_settings[this->scene_item_selected][6].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        if (ImGui::Checkbox("##2", &this->scene_item_settings[this->scene_item_selected][7].oAnimate))
            this->animateValue(false, this->scene_item_selected, 7, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##103", &this->scene_item_settings[this->scene_item_selected][7].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][8].oAnimate))
            this->animateValue(false, this->scene_item_selected, 8, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate translation by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##103", &this->scene_item_settings[this->scene_item_selected][8].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Displace")) {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][9].oAnimate))
            this->animateValue(false, this->scene_item_selected, 9, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate displacement by X");
        ImGui::SameLine(); ImGui::SliderFloat("X##103", &this->scene_item_settings[this->scene_item_selected][9].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        if (ImGui::Checkbox("##2", &this->scene_item_settings[this->scene_item_selected][10].oAnimate))
            this->animateValue(false, this->scene_item_selected, 10, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate displacement by Y");
        ImGui::SameLine(); ImGui::SliderFloat("Y##103", &this->scene_item_settings[this->scene_item_selected][10].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][11].oAnimate))
            this->animateValue(false, this->scene_item_selected, 11, 0.05f, this->so_GUI_grid_size, true);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate displacement by Z");
        ImGui::SameLine(); ImGui::SliderFloat("Z##103", &this->scene_item_settings[this->scene_item_selected][11].oValue, -1 * this->so_GUI_grid_size, this->so_GUI_grid_size);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Material")) {
        if (ImGui::Checkbox("##1", &this->scene_item_settings[this->scene_item_selected][12].oAnimate))
            this->animateValue(false, this->scene_item_selected, 12, 0.05f, 10, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate refraction");
        ImGui::SameLine(); ImGui::SliderFloat("Refraction", &this->scene_item_settings[this->scene_item_selected][12].oValue, 1.0, 10.0);

        if (ImGui::Checkbox("##3", &this->scene_item_settings[this->scene_item_selected][13].oAnimate))
            this->animateValue(false, this->scene_item_selected, 13, 0.05f, 10, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate shininess");
        ImGui::SameLine(); ImGui::SliderFloat("Shininess", &this->scene_item_settings[this->scene_item_selected][13].oValue, 0.0, 10.0);

        ImGui::TextColored(ImVec4(this->scene_item_settings[this->scene_item_selected][14].vValue.r, this->scene_item_settings[this->scene_item_selected][14].vValue.g, this->scene_item_settings[this->scene_item_selected][14].vValue.b, 1.0), "Ambient");
        ImGui::SliderFloat("Red##101", &this->scene_item_settings[this->scene_item_selected][14].vValue.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##102", &this->scene_item_settings[this->scene_item_selected][14].vValue.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##103", &this->scene_item_settings[this->scene_item_selected][14].vValue.b, 0.0f, 1.0f);

        ImGui::TextColored(ImVec4(this->scene_item_settings[this->scene_item_selected][15].vValue.r, this->scene_item_settings[this->scene_item_selected][15].vValue.g, this->scene_item_settings[this->scene_item_selected][15].vValue.b, 1.0), "Diffuse");
        ImGui::SliderFloat("Red##104", &this->scene_item_settings[this->scene_item_selected][15].vValue.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##105", &this->scene_item_settings[this->scene_item_selected][15].vValue.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##106", &this->scene_item_settings[this->scene_item_selected][15].vValue.b, 0.0f, 1.0f);

        ImGui::TextColored(ImVec4(this->scene_item_settings[this->scene_item_selected][16].vValue.r, this->scene_item_settings[this->scene_item_selected][16].vValue.g, this->scene_item_settings[this->scene_item_selected][16].vValue.b, 1.0), "Specular");
        ImGui::SliderFloat("Red##107", &this->scene_item_settings[this->scene_item_selected][16].vValue.r, 0.0f, 1.0f);
        ImGui::SliderFloat("Green##108", &this->scene_item_settings[this->scene_item_selected][16].vValue.g, 0.0f, 1.0f);
        ImGui::SliderFloat("Blue##109", &this->scene_item_settings[this->scene_item_selected][16].vValue.b, 0.0f, 1.0f);

        ImGui::TreePop();
    }

    ImGui::End();
}

void GUI::animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    std::thread animThread(&GUI::animateValueAsync, this, isGUI, elementID, sett_index, step, limit, doMinus);
    animThread.detach();
}

void GUI::animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    if (isGUI) {
        while (this->gui_item_settings[elementID][sett_index].oAnimate) {
            if (this->isFrame) {
                float v = this->gui_item_settings[elementID][sett_index].oValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                this->gui_item_settings[elementID][sett_index].oValue  = v;
                this->isFrame = false;
            }
        }
    }
    else {
        while (this->scene_item_settings[elementID][sett_index].oAnimate) {
            if (this->isFrame) {
                float v = this->scene_item_settings[elementID][sett_index].oValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                this->scene_item_settings[elementID][sett_index].oValue  = v;
                this->isFrame = false;
            }
        }
    }
}

void GUI::resetValuesSceneSettings() {
    this->so_Alpha = 1;
    for (int i=0; i<(int)this->scene_item_settings.size(); i++) {
        for (int j=0; j<(int)this->scene_item_settings[i].size(); j++) {
            this->scene_item_settings[i][j] = this->scene_item_settings_default[i][j];
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
                gui_MouseWheel = 1;
            if (event->wheel.y < 0)
                gui_MouseWheel = -1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event->button.button == SDL_BUTTON_LEFT) gui_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT) gui_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE) gui_MousePressed[2] = true;
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

    glGenTextures(1, &gui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, gui_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)gui_FontTexture;

    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();

    return true;
}

void GUI::ImGui_SDL2GL21_Implementation_InvalidateDeviceObjects() {
    if (gui_FontTexture) {
        glDeleteTextures(1, &gui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        gui_FontTexture = 0;
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
    if (!gui_FontTexture)
        this->ImGui_SDL2GL21_Implementation_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    SDL_GetWindowSize(this->sdlWindow, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);

    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = gui_Time > 0.0 ? (float)(current_time - gui_Time) : (float)(1.0f/60.0f);
    gui_Time = current_time;

    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);
    else
        io.MousePos = ImVec2(-1,-1);

    io.MouseDown[0] = gui_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = gui_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = gui_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    gui_MousePressed[0] = gui_MousePressed[1] = gui_MousePressed[2] = false;

    io.MouseWheel = gui_MouseWheel;
    gui_MouseWheel = 0.0f;

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
    glUseProgram(gui_ShaderHandle);
    glUniform1i(gui_AttribLocationTex, 0);
    glUniformMatrix4fv(gui_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(gui_VaoHandle);

    for (int n=0; n<draw_data->CmdListsCount; n++) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, gui_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui_ElementsHandle);
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
    if (!gui_FontTexture)
        this->ImGui_SDL2GL32_Implementation_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    int w, h;
    SDL_GetWindowSize(this->sdlWindow, &w, &h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    Uint32	time = SDL_GetTicks();
    double current_time = time / 1000.0;
    io.DeltaTime = gui_Time > 0.0 ? (float)(current_time - gui_Time) : (float)(1.0f / 60.0f);
    gui_Time = current_time;

    int mx, my;
    Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
    if (SDL_GetWindowFlags(this->sdlWindow) & SDL_WINDOW_MOUSE_FOCUS)
        io.MousePos = ImVec2((float)mx, (float)my);
    else
        io.MousePos = ImVec2(-1, -1);

    io.MouseDown[0] = gui_MousePressed[0] || (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    io.MouseDown[1] = gui_MousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = gui_MousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    gui_MousePressed[0] = gui_MousePressed[1] = gui_MousePressed[2] = false;

    io.MouseWheel = gui_MouseWheel;
    gui_MouseWheel = 0.0f;

    SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);
    ImGui::NewFrame();
}

bool GUI::ImGui_SDL2GL32_Implementation_ProcessEvent(SDL_Event* event) {
    ImGuiIO& io = ImGui::GetIO();
    switch (event->type) {
        case SDL_MOUSEWHEEL: {
            if (event->wheel.y > 0)
                gui_MouseWheel = 1;
            if (event->wheel.y < 0)
                gui_MouseWheel = -1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN: {
            if (event->button.button == SDL_BUTTON_LEFT)
                gui_MousePressed[0] = true;
            if (event->button.button == SDL_BUTTON_RIGHT)
                gui_MousePressed[1] = true;
            if (event->button.button == SDL_BUTTON_MIDDLE)
                gui_MousePressed[2] = true;
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
    if (gui_VaoHandle)
        glDeleteVertexArrays(1, &gui_VaoHandle);
    if (gui_VboHandle)
        glDeleteBuffers(1, &gui_VboHandle);
    if (gui_ElementsHandle)
        glDeleteBuffers(1, &gui_ElementsHandle);
    gui_VaoHandle = gui_VboHandle = gui_ElementsHandle = 0;

    glDetachShader(gui_ShaderHandle, gui_VertHandle);
    glDeleteShader(gui_VertHandle);
    gui_VertHandle = 0;

    glDetachShader(gui_ShaderHandle, gui_FragHandle);
    glDeleteShader(gui_FragHandle);
    gui_FragHandle = 0;

    glDeleteProgram(gui_ShaderHandle);
    gui_ShaderHandle = 0;

    if (gui_FontTexture) {
        glDeleteTextures(1, &gui_FontTexture);
        ImGui::GetIO().Fonts->TexID = 0;
        gui_FontTexture = 0;
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

    gui_ShaderHandle = glCreateProgram();
    gui_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    gui_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(gui_VertHandle, 1, &vertex_shader, 0);
    glShaderSource(gui_FragHandle, 1, &fragment_shader, 0);
    glCompileShader(gui_VertHandle);
    glCompileShader(gui_FragHandle);
    glAttachShader(gui_ShaderHandle, gui_VertHandle);
    glAttachShader(gui_ShaderHandle, gui_FragHandle);
    glLinkProgram(gui_ShaderHandle);

    gui_AttribLocationTex = glGetUniformLocation(gui_ShaderHandle, "Texture");
    gui_AttribLocationProjMtx = glGetUniformLocation(gui_ShaderHandle, "ProjMtx");
    gui_AttribLocationPosition = glGetAttribLocation(gui_ShaderHandle, "Position");
    gui_AttribLocationUV = glGetAttribLocation(gui_ShaderHandle, "UV");
    gui_AttribLocationColor = glGetAttribLocation(gui_ShaderHandle, "Color");

    glGenBuffers(1, &gui_VboHandle);
    glGenBuffers(1, &gui_ElementsHandle);

    glGenVertexArrays(1, &gui_VaoHandle);
    glBindVertexArray(gui_VaoHandle);
    glBindBuffer(GL_ARRAY_BUFFER, gui_VboHandle);
    glEnableVertexAttribArray(gui_AttribLocationPosition);
    glEnableVertexAttribArray(gui_AttribLocationUV);
    glEnableVertexAttribArray(gui_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(gui_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(gui_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(gui_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
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

    glGenTextures(1, &gui_FontTexture);
    glBindTexture(GL_TEXTURE_2D, gui_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (void *)(intptr_t)gui_FontTexture;

    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
}
