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
#include "utilities/gui/Objects.h"
#include "utilities/gui/imgui/imgui.h"
#include "utilities/gui/components/Log.hpp"
#include "utilities/gui/components/Screenshot.hpp"
#include "utilities/gui/components/FileBrowser.hpp"
#include "utilities/gui/components/Editor.hpp"
#include "utilities/gui/components/ColorPicker.hpp"
#include "utilities/gui/dialogs/DialogStyle.hpp"
#include "utilities/gui/dialogs/DialogOptions.hpp"
#include "utilities/gui/dialogs/DialogGUIControls.hpp"
#include "utilities/gui/implementation/SDL2OpenGL32.hpp"
#include "kuplung/meshes/MeshModelFace.hpp"

#define STBI_FAILURE_USERMSG
#include "utilities/stb/stb_image.h"

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
    void setHeightmapImage(std::string heightmapImage);
    void setShaderEditor(std::function<void(std::string)> fileShaderCompile);
    bool isMouseOnGUI();

    void showLoading();
    void hideLoading();

    void initGUIControls(int guiObjectsCount, std::vector<std::vector<float>> initialSettings);
    void showGUIControls();
    void hideGUIControls();
    void hideSceneSettings();
    void showSceneStats();
    void hideSceneStats();

    void addSceneModelSettings();
    void removeSceneModelSettings(int idx);
    void setModelSetting(int modelID, int settingID, int iValue = 0, float fValue = 0, bool bValue = true, glm::vec4 vValue = glm::vec4(1, 1, 1, 1));

    void addSceneLight();

    std::vector<MeshModelFace*> * meshModelFaces;
    bool isFrame, isProjection, fixedGridWorld, showHeightmap, isLoadingOpen;
    bool displayGUIControls, displaySceneStats, displaySceneSettings;

    int gui_item_selected;
    std::vector<std::vector<GUIObjectSetting*>> gui_item_settings, gui_item_settings_default;
    int scene_item_selected;
    std::vector<std::vector<GUIObjectSetting*>> scene_item_settings, scene_item_settings_default;

    float so_GUI_FOV = 45.0, so_Alpha = 1, so_outlineThickness = 1.01;
    float so_GUI_ratio_w = 4.0f, so_GUI_ratio_h = 3.0f, so_GUI_plane_close = 0.1f, so_GUI_plane_far = 100.0f;
    int so_GUI_grid_size = 10;
    glm::vec4 so_GUI_outlineColor;
    float loadingPercentage;

    int sceneLightsSelected;
    std::vector<GUISceneLight*> sceneLights;

private:
    std::function<void()> quitApp;
    std::function<void(FBEntity)> processFile;
    std::function<void()> newScene;
    std::function<void(std::string)> doFileShaderCompile;

    void dialogScreenshot();
    void dialogLog();
    void dialogMetrics();
    void dialogAboutImGui();
    void dialogAboutKuplung();
    void dialogFileBrowser();
    void dialogStyleBrowser();
    void dialogFileBrowserProcessFile(FBEntity file);
    void dialogOptions(ImGuiStyle* ref = NULL);
    void dialogHeightmap();
    void dialogEditor();

    void contextModelRename();
    void contextModelDelete();

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
    void addControlsSlider(std::string title, int idx, bool isGUI, float step, float limit, bool showAnimate, bool* animate, float* sliderValue);
    void addControlColor3(std::string title, glm::vec3* vValue, bool* bValue);
    void addControlColor4(std::string title, glm::vec4* vValue, bool* bValue);

    SDL_Window *sdlWindow;
    bool isGUIVisible;
    bool logDebugInfo, showScreenshotWindow, showFileDialog, showStyleDialog, showOptions, showEditor;
    bool showAppMetrics, showAboutKuplung, showAboutImgui;
    bool showDemoWindow, newHeightmap;

    SDL2OpenGL32 *imguiImplementation;

    Log *componentLog;
    Screenshot *componentScreenshot;
    FileBrowser *componentFileBrowser;
    Editor *componentFileEditor;
    ColorPicker *componentColorPicker;
    DialogStyle *windowStyle;
    DialogOptions *windowOptions;
    DialogGUIControls *windowGUIControls;

    std::string heightmapImage;
    std::map <std::string, FBEntity> recentFiles;
    GLuint vboTexHeightmap;
    int heightmapWidth, heightmapHeight;
    int selectedTabScene, selectedTabGUICamera, selectedTabGUIGrid, selectedTabGUILight, selectedTabGUITerrain;
    bool outlineColorPickerOpen, cmenu_deleteYn, cmenu_renameModel, needsFontChange;
    char guiModelRenameText[256];
};

#endif /* GUI_hpp */
