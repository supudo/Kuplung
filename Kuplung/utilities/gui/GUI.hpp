//
//  gui.hpp
// Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef gui_hpp
#define gui_hpp

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

#define STBI_FAILURE_USERMSG
#include "utilities/stb/stb_image.h"

struct GUIObjectSetting {
    int oIndex;
    bool oAnimate;
    float oValue;
    glm::vec3 vValue;
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
    void setShaderEditor(std::function<void(std::string, std::string)> fileShaderCompile);

    void showLoading();
    void hideLoading();

    void initGUIControls(int guiObjectsCount, std::map<int, std::vector<float>> initialSettings);
    void showGUIControls();
    void hideGUIControls();
    void showSceneSettings(std::map<int, std::string> scene_models);
    void hideSceneSettings();
    void showSceneStats();
    void hideSceneStats();

    void setModelOSetting(int modelID, int settingID, float oValue);
    void setModelVSetting(int modelID, int settingID, glm::vec3 vValue);

    bool isFrame, isProjection, fixedGridWorld, showHeightmap;

    int gui_item_selected;
    std::map<int, std::vector<GUIObjectSetting*>> gui_item_settings, gui_item_settings_default;
    int scene_item_selected;
    std::map<int, std::vector<GUIObjectSetting*>> scene_item_settings, scene_item_settings_default;

    float so_GUI_FOV = 45.0, so_Alpha = 1, so_outlineThickness = 1.01;
    float so_GUI_ratio_w = 4.0f, so_GUI_ratio_h = 3.0f, so_GUI_plane_close = 0.1f, so_GUI_plane_far = 100.0f;
    int so_GUI_grid_size = 10;
    glm::vec3 so_GUI_outlineColor;
    float loadingPercentage;

    glm::vec3 so_GUI_lightAmbient, so_GUI_lightDiffuse, so_GUI_lightSpecular;
    float so_GUI_lightAmbientStrength, so_GUI_lightDiffuseStrength, so_GUI_lightSpecularStrength;
    glm::vec3 so_GUI_materialAmbient, so_GUI_materialDiffuse, so_GUI_materialSpecular;

private:
    std::function<void()> quitApp;
    std::function<void(FBEntity)> processFile;
    std::function<void()> newScene;
    std::function<void(std::string, std::string)> doFileShaderCompile;
    std::map<int, std::string> sceneModels;
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

    GUIObjectSetting* addSceneSettingsObject(int idx, float oValue, glm::vec3 vValue);
    void dialogGUIControls();
    void resetValuesGUIControls();
    void dialogSceneSettings();
    void resetValuesSceneSettings();
    void animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);
    void animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);
    void fileEditorSaved(std::string fileName, std::string fileSource);

    void dialogSceneStats();

    SDL_Window *sdlWindow;
    bool isGUIVisible, isLoadingOpen;
    bool logDebugInfo, showScreenshotWindow, showFileDialog, showOptions, showEditor;
    bool showAppMetrics, showAboutKuplung, showAboutImgui;
    bool displayGUIControls, displaySceneStats, displaySceneSettings;
    bool showDemoWindow, newHeightmap;

    GUILog *windowLog;
    GUIScreenshot *windowScreenshot;
    GUIFileBrowser *windowFileBrowser;
    GUIEditor *fileEditor;

    std::map <std::string, FBEntity> recentFiles;
    GLuint vboTexHeightmap;
    int heightmapWidth, heightmapHeight;
};

#endif /* gui_hpp */
