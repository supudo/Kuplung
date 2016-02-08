//
//  DialogControls.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogControls_hpp
#define DialogControls_hpp

#include <regex>
#include <string>
#include <thread>
#include <vector>
#include <glm/glm.hpp>
#include "utilities/gui/ImGui/imgui.h"
#include "utilities/gui/components/ColorPicker.hpp"
#include "utilities/gui/Objects.h"

class DialogControls {
public:
    void init(std::function<void(std::string)> doLog);
    void showGUIControls(bool* displayGUIControls,
                         std::function<void()> resetValuesGUIControls,
                         bool* isFameParent,
                         std::vector<std::vector<GUIObjectSetting*>> gui_item_settings,
                         std::vector<GUISceneLight*> sceneLights,
                         int* gui_item_selected,
                         float* so_GUI_FOV,
                         float* so_GUI_ratio_w,
                         float* so_GUI_ratio_h,
                         float* so_GUI_plane_close,
                         float* so_GUI_plane_far,
                         int* so_GUI_grid_size,
                         bool* fixedGridWorld,
                         glm::vec4* so_GUI_outlineColor,
                         bool* outlineColorPickerOpen,
                         float* so_outlineThickness,
                         int* selectedTabGUICamera,
                         int* selectedTabGUIGrid,
                         int* selectedTabGUILight,
                         int* selectedTabGUITerrain,
                         bool* showHeightmap);

private:
    std::function<void(std::string)> doLog;

    void addControlsXYZ(bool isGuiControl, int x, int y, int z, std::string animate, float animateStep, float animateLimit);
    void addControlsSlider(std::string title, int idx, bool isGUI, float step, float limit, bool showAnimate, bool* animate, float* sliderValue);
    void addControlColor3(std::string title, glm::vec3* vValue, bool* bValue);
    void addControlColor4(std::string title, glm::vec4* vValue, bool* bValue);

    void animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);
    void animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus);

    void logMessage(std::string message);

    ColorPicker *componentColorPicker;

    bool* isFrame;
    int* gui_item_selected;
    std::vector<std::vector<GUIObjectSetting*>> gui_item_settings, gui_item_settings_default;
    int* scene_item_selected;
    std::vector<std::vector<GUIObjectSetting*>> scene_item_settings, scene_item_settings_default;

    int sceneLightsSelected;
    std::vector<GUISceneLight*> sceneLights;

    float so_GUI_FOV = 45.0, so_Alpha = 1, so_outlineThickness = 1.01;
    float so_GUI_ratio_w = 4.0f, so_GUI_ratio_h = 3.0f, so_GUI_plane_close = 0.1f, so_GUI_plane_far = 100.0f;
    int so_GUI_grid_size = 10;
    glm::vec4 so_GUI_outlineColor;
};

#endif /* DialogControls_hpp */
