//
//  gui.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef GUI_hpp
#define GUI_hpp

#include <SDL2/SDL.h>
#include <functional>
#include <map>
#include <string>
#include <thread>
#include <glm/glm.hpp>
#include "imgui/imgui.h"
#include "GUILog.hpp"
#include "GUIScreenshot.hpp"
#include "GUIFileBrowser.hpp"
#include "GUIEditor.hpp"
#include "GUIColorPicker.hpp"

#define STBI_FAILURE_USERMSG
#include "utilities/stb/stb_image.h"

struct GUIObjectSetting {
    int oIndex;
    bool oAnimate;
    int iValue;
    float fValue;
    bool bValue;
    glm::vec4 vValue;
};

struct GUISceneObject {
    std::string objFile;
    std::string modelID;
    std::string materialID;
    int verticesCount;
    int normalsCount;
    int indicesCount;
};

struct GUILightObject {
    bool colorPickerOpen;
    float strength;
    glm::vec3 color;
};

struct GUISceneLight {
    GUILightObject *ambient;
    GUILightObject *diffuse;
    GUILightObject *specular;
};

class GUI {
public:
    ~GUI();
    void init(SDL_Window *window, std::function<void()> quitApp, std::function<void(FBEntity)> processFile, std::function<void()> newScene);
    bool processEvent(SDL_Event *event);
    void renderStart(bool isFrame);
    void renderEnd();
    void destroy();
    void doLog(std::string logMessage);
    void recentFilesAdd(std::string title, FBEntity file);
    void recentFilesClear();
    void ImGui_Implementation_RenderDrawLists();
    void setHeightmapImage(std::string heightmapImage);
    void setShaderEditor(std::function<void(std::string)> fileShaderCompile);
    bool isMouseOnGUI();

    void showLoading();
    void hideLoading();

    void initGUIControls(int guiObjectsCount, std::map<int, std::vector<float>> initialSettings);
    void showGUIControls();
    void hideGUIControls();
    void hideSceneSettings();
    void showSceneStats();
    void hideSceneStats();

    void addSceneModelSettings(std::string objFile, std::string modelID, std::string materialID, int verticesCount, int normalsCount, int indicesCount);
    void setModelSetting(int modelID, int settingID, int iValue = 0, float fValue = 0, bool bValue = true, glm::vec4 vValue = glm::vec4(1, 1, 1, 1));

    void addSceneLight();

    //std::map<int, std::string> sceneModels;
    std::vector<GUISceneObject> sceneModels;
    bool isFrame, isProjection, fixedGridWorld, showHeightmap, isLoadingOpen;
    bool displayGUIControls, displaySceneStats, displaySceneSettings;

    int gui_item_selected;
    std::map<int, std::vector<GUIObjectSetting*>> gui_item_settings, gui_item_settings_default;
    int scene_item_selected;
    std::map<int, std::vector<GUIObjectSetting*>> scene_item_settings, scene_item_settings_default;

    float so_GUI_FOV = 45.0, so_Alpha = 1, so_outlineThickness = 1.01;
    float so_GUI_ratio_w = 4.0f, so_GUI_ratio_h = 3.0f, so_GUI_plane_close = 0.1f, so_GUI_plane_far = 100.0f;
    int so_GUI_grid_size = 10;
    glm::vec3 so_GUI_outlineColor;
    float loadingPercentage;

    int sceneLightsSelected;
    std::vector<GUISceneLight*> sceneLights;

private:
    std::function<void()> quitApp;
    std::function<void(FBEntity)> processFile;
    std::function<void()> newScene;
    std::function<void(std::string)> doFileShaderCompile;
    std::string heightmapImage;

    bool ImGui_SDL2GL21_Implementation_Init();
    void ImGui_SDL2GL21_Implementation_Shutdown();
    void ImGui_SDL2GL21_Implementation_NewFrame();
    bool ImGui_SDL2GL21_Implementation_ProcessEvent(SDL_Event* event);
    void ImGui_SDL2GL21_Implementation_InvalidateDeviceObjects();
    bool ImGui_SDL2GL21_Implementation_CreateDeviceObjects();
    void ImGui_SDL2GL21_Implementation_RenderDrawLists();

    bool ImGui_SDL2GL32_Implementation_Init();
    void ImGui_SDL2GL32_Implementation_Shutdown();
    void ImGui_SDL2GL32_Implementation_NewFrame();
    bool ImGui_SDL2GL32_Implementation_ProcessEvent(SDL_Event* event);
    void ImGui_SDL2GL32_Implementation_InvalidateDeviceObjects();
    bool ImGui_SDL2GL32_Implementation_CreateDeviceObjects();
    void ImGui_SDL2GL32_Implementation_CreateFontsTexture();
    void ImGui_SDL2GL32_Implementation_RenderDrawLists();

    void dialogScreenshot();
    void dialogLog();
    void dialogMetrics();
    void dialogAboutImGui();
    void dialogAboutKuplung();
    void dialogFileBrowser();
    void dialogFileBrowserProcessFile(FBEntity file);
    void dialogOptions();
    void dialogHeightmap();
    void dialogEditor();

    GUIObjectSetting* addSceneSettingsObject(int idx, float fValue = 1.0f, bool bValue = true, glm::vec4 vValue = glm::vec4(1.0, 1.0, 1.0, 1.0));
    void dialogGUIControls();
    void resetValuesGUIControls();
    void dialogSceneSettings();
    void resetValuesSceneSettings();
    void animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);
    void animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);
    void fileEditorSaved(std::string fileName);

    void dialogSceneStats();

    void addControlsXYZ(bool isGuiControl, int x, int y, int z, std::string animate, float animateStep, float animateLimit);

    SDL_Window *sdlWindow;
    bool isGUIVisible;
    bool logDebugInfo, showScreenshotWindow, showFileDialog, showOptions, showEditor;
    bool showAppMetrics, showAboutKuplung, showAboutImgui;
    bool showDemoWindow, newHeightmap;

    GUILog *windowLog;
    GUIScreenshot *windowScreenshot;
    GUIFileBrowser *windowFileBrowser;
    GUIEditor *fileEditor;
    GUIColorPicker *colorPicker;

    std::map <std::string, FBEntity> recentFiles;
    GLuint vboTexHeightmap;
    int heightmapWidth, heightmapHeight;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight, selectedTabGUITerrain;
    bool outlineColorPickerOpen;
};

#endif /* GUI_hpp */
