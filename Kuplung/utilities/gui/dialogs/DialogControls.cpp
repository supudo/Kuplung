//
//  DialogControls.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "DialogControls.hpp"
#include "utilities/gui/Objects.h"
#include "utilities/gui/ImGui/imgui_internal.h"
#include "utilities/gui/components/Tabs.hpp"
#include "utilities/gui/components/IconsFontAwesome.h"
#include "utilities/gui/components/IconsMaterialDesign.h"
#include "utilities/settings/Settings.h"

void DialogControls::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
    this->componentColorPicker = new ColorPicker();
}

void DialogControls::logMessage(std::string message) {
    this->doLog("[DialogGUIControls] " + message);
}

void DialogControls::showGUIControls(bool* displayGUIControls,
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
                                     int* sceneLightsSelected) {
    this->isFrame = isFameParent;
    this->gui_item_settings = gui_item_settings;
    this->gui_item_selected = gui_item_selected;

    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("GUI Controls", &(*displayGUIControls), ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->resetValuesGUIControls();
    ImGui::PopStyleColor(3);
    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);

    const char* gui_items[] = { "General", "Camera", "Grid", "Light", "Terrain" };
    ImGui::ListBox("", &(*gui_item_selected), gui_items, IM_ARRAYSIZE(gui_items));
    ImGui::PopItemWidth();

    ImGui::Separator();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
    switch (*gui_item_selected) {
        case 0: {
            this->addControlsSlider("Field of view", 1, true, 0.0f, 180.0f, false, NULL, &(*so_GUI_FOV));
            ImGui::Separator();

            ImGui::Text("Ratio"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("W & H");
            ImGui::SliderFloat("W##105", &(*so_GUI_ratio_w), 0.0f, 5.0f);
            ImGui::SliderFloat("H##106", &(*so_GUI_ratio_h), 0.0f, 5.0f);
            ImGui::Separator();

            ImGui::Text("Planes"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Far & Close");
            ImGui::SliderFloat("Far##107", &(*so_GUI_plane_close), 0.0f, 1.0f);
            ImGui::SliderFloat("Close##108", &(*so_GUI_plane_far), 0.0f, 100.0f);
            ImGui::Separator();

            ImGui::Text("Grid size"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Squares");
            ImGui::SliderInt("##109", &(*so_GUI_grid_size), 0, 100);
            ImGui::Separator();

            ImGui::Checkbox("Grid fixed with World", &(*fixedGridWorld));
            ImGui::Separator();

            this->addControlColor4("Outline Color", &(*so_GUI_outlineColor), &(*outlineColorPickerOpen));
            this->addControlsSlider("Outline Thickness", 0, true, 1.01f, 2.0f, false, NULL, &(*so_outlineThickness));
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
            ImGui::TabLabels(numTabsGUICamera, tabsGUICamera, *selectedTabGUICamera, ImVec2(30.0, 30.0), tabsLabelsGUICamera);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            std::vector<GUIObjectSetting*> v = gui_item_settings[*gui_item_selected];

            switch (*selectedTabGUICamera) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Look-At matrix");
                    ImGui::SliderFloat("Eye X", &v[0]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Eye Y", &v[1]->fValue, -100.0f, 10.0f);
                    ImGui::SliderFloat("Eye Z", &v[2]->fValue, 0.0f, 90.0f);
                    ImGui::Separator();
                    ImGui::SliderFloat("Center X", &v[3]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Center Y", &v[4]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Center Z", &v[5]->fValue, 0.0f, 45.0f);
                    ImGui::Separator();
                    ImGui::SliderFloat("Up X", &v[6]->fValue, -10.0f, 10.0f);
                    ImGui::SliderFloat("Up Y", &v[7]->fValue, -1.0f, 1.0f);
                    ImGui::SliderFloat("Up Z", &v[8]->fValue, -10.0f, 10.0f);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    if (ImGui::Checkbox("##1", &v[12]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 12, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##2", &v[12]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##2", &v[13]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 13, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##2", &v[13]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##3", &v[14]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 14, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##2", &v[14]->fValue, 0.0f, 360.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    if (ImGui::Checkbox("##1", &v[15]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 15, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##3", &v[15]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
                    if (ImGui::Checkbox("##2", &v[16]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 16, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##3", &v[16]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
                    if (ImGui::Checkbox("##3", &v[17]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 17, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##3", &v[17]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
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
            ImGui::TabLabels(numTabsGUIGrid, tabsGUIGrid, *selectedTabGUIGrid, ImVec2(30.0, 30.0), tabsLabelsGUIGrid);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            std::vector<GUIObjectSetting*> v = gui_item_settings[*gui_item_selected];

            switch (*selectedTabGUIGrid) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    ImGui::SliderFloat("X##1", &v[9]->fValue, 0.0f, 1.0f);
                    ImGui::SliderFloat("Y##1", &v[10]->fValue, 0.0f, 1.0f);
                    ImGui::SliderFloat("Z##1", &v[11]->fValue, 0.0f, 1.0f);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    if (ImGui::Checkbox("##1", &v[12]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 12, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##2", &v[12]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##2", &v[13]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 13, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##2", &v[13]->fValue, 0.0f, 360.0f);
                    if (ImGui::Checkbox("##3", &v[14]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 14, 1.0f, 360.0, false);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate rotation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##2", &v[14]->fValue, 0.0f, 360.0f);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    if (ImGui::Checkbox("##1", &v[15]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 15, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by X");
                    ImGui::SameLine(); ImGui::SliderFloat("X##3", &v[15]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
                    if (ImGui::Checkbox("##2", &v[16]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 16, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Y");
                    ImGui::SameLine(); ImGui::SliderFloat("Y##3", &v[16]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
                    if (ImGui::Checkbox("##3", &v[17]->oAnimate))
                        this->animateValue(true, *gui_item_selected, 17, 0.05f, *so_GUI_grid_size, true);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("Animate translation by Z");
                    ImGui::SameLine(); ImGui::SliderFloat("Z##3", &v[17]->fValue, -1 * (*so_GUI_grid_size), *so_GUI_grid_size);
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
            ImGui::TabLabels(numTabsGUILight, tabsGUILight, *selectedTabGUILight, ImVec2(30.0, 30.0), tabsLabelsGUILight);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            std::vector<GUIObjectSetting*> v = gui_item_settings[*gui_item_selected];

            switch (*selectedTabGUILight) {
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
                    this->addControlsXYZ(true, 15, 16, 17, "translation", 0.05f, *so_GUI_grid_size);
                    break;
                }
                case 4: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Light colors");

                    this->addControlColor3("Ambient Color", &sceneLights[0]->ambient->color, &sceneLights[0]->ambient->colorPickerOpen);
                    this->addControlsSlider("Ambient Strength", 18, true, 0.1f, 1.0f, true, &v[18]->oAnimate, &v[18]->fValue);

                    this->addControlColor3("Diffuse Color", &sceneLights[0]->diffuse->color, &sceneLights[0]->diffuse->colorPickerOpen);
                    this->addControlsSlider("Diffuse Strength", 19, true, 0.1f, 4.0f, true, &v[19]->oAnimate, &v[19]->fValue);

                    this->addControlColor3("Specular Color", &sceneLights[0]->specular->color, &sceneLights[0]->specular->colorPickerOpen);
                    this->addControlsSlider("Specular Strength", 20, true, 0.1f, 4.0f, true, &v[20]->oAnimate, &v[20]->fValue);
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
            ImGui::TabLabels(numTabsGUITerrain, tabsGUITerrain, *selectedTabGUITerrain, ImVec2(30.0, 30.0), tabsLabelsGUITerrain);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (*selectedTabGUITerrain) {
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

void DialogControls::resetValuesGUIControls() {
//    this->so_GUI_outlineColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
//    this->sceneLights[0]->ambient = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1, 1, 1) });
//    this->sceneLights[0]->diffuse = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1, 1, 1) });
//    this->sceneLights[0]->specular = new GUILightObject({ /*.colorPickerOpen=*/ false, /*.strength=*/ 0.0, /*.color=*/ glm::vec3(1, 1, 1) });

//    this->so_GUI_FOV = 45.0;
//    this->so_GUI_ratio_w = 4.0f;
//    this->so_GUI_ratio_h = 3.0f;
//    this->so_GUI_plane_close = 0.1f;
//    this->so_GUI_plane_far = 100.0f;
//    this->so_GUI_grid_size = 10;
//    this->so_outlineThickness = 1.01;

//    for (int i=0; i<(int)this->gui_item_settings_default.size(); i++) {
//        for (int j=0; j<(int)this->gui_item_settings[i].size(); j++) {
//            this->gui_item_settings[i][j]->fValue = this->gui_item_settings_default[i][j]->fValue;
//            this->gui_item_settings[i][j]->vValue = this->gui_item_settings_default[i][j]->vValue;
//        }
//    }
}

void DialogControls::addControlsXYZ(bool isGuiControl, int x, int y, int z, std::string animate, float animateStep, float animateLimit) {
    if (isGuiControl) {
        std::vector<GUIObjectSetting*> v = this->gui_item_settings[*this->gui_item_selected];

        if (ImGui::Checkbox("##1", &v[x]->oAnimate))
            this->animateValue(true, *this->gui_item_selected, x, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by X", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("X##2", &v[x]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##2", &v[y]->oAnimate))
            this->animateValue(true, *this->gui_item_selected, y, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Y", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Y##2", &v[y]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##3", &v[z]->oAnimate))
            this->animateValue(true, *this->gui_item_selected, z, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Z", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Z##2", &v[z]->fValue, 0.0f, animateLimit);
    }
    else {
        std::vector<GUIObjectSetting*> v = this->scene_item_settings[*this->scene_item_selected];

        if (ImGui::Checkbox("##1", &v[x]->oAnimate))
            this->animateValue(false, *this->scene_item_selected, x, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by X", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("X##101", &v[x]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##2", &v[y]->oAnimate))
            this->animateValue(false, *this->scene_item_selected, y, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Y", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Y##101", &v[y]->fValue, 0.0f, animateLimit);

        if (ImGui::Checkbox("##3", &v[z]->oAnimate))
            this->animateValue(false, *this->scene_item_selected, z, animateStep, animateLimit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s by Z", animate.c_str());
        ImGui::SameLine(); ImGui::SliderFloat("Z##101", &v[z]->fValue, 0.0f, animateLimit);
    }
}

void DialogControls::addControlsSlider(std::string title, int idx, bool isGUI, float step, float limit, bool showAnimate, bool* animate, float* sliderValue) {
    if (title != "")
        ImGui::Text("%s", title.c_str());
    if (showAnimate) {
        std::string c_id = "##00" + std::to_string(idx);
        if (ImGui::Checkbox(c_id.c_str(), *(&animate)))
            this->animateValue(isGUI, (isGUI ? *this->gui_item_selected : *this->scene_item_selected), idx, step, limit, false);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Animate %s", title.c_str());
        ImGui::SameLine();
    }
    std::string s_id = "##10" + std::to_string(idx);
    ImGui::SliderFloat(s_id.c_str(), *(&sliderValue), 0.0, limit);
}

void DialogControls::addControlColor3(std::string title, glm::vec3* vValue, bool* bValue) {
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

void DialogControls::addControlColor4(std::string title, glm::vec4* vValue, bool* bValue) {
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

void DialogControls::animateValue(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    std::thread animThread(&DialogControls::animateValueAsync, this, isGUI, elementID, sett_index, step, limit, doMinus);
    animThread.detach();
}

void DialogControls::animateValueAsync(bool isGUI, int elementID, int sett_index, float step, float limit, bool doMinus) {
    if (isGUI) {
        std::vector<GUIObjectSetting*> vv = this->gui_item_settings[elementID];
        while (vv[sett_index]->oAnimate) {
            if (this->isFrame) {
                float v = vv[sett_index]->fValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                vv[sett_index]->fValue = v;
                *this->isFrame = false;
            }
        }
    }
    else {
        std::vector<GUIObjectSetting*> vv = this->scene_item_settings[elementID];
        while (vv[sett_index]->oAnimate) {
            if (this->isFrame) {
                float v = vv[sett_index]->fValue;
                v += step;
                if (v > limit)
                    v = (doMinus ? -1 * limit : 0);
                vv[sett_index]->fValue = v;
                *this->isFrame = false;
            }
        }
    }
}
