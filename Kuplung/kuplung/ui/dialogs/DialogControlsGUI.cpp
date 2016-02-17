//
//  DialogControlsGUI.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogControlsGUI.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/ui/components/Tabs.hpp"

void DialogControlsGUI::init(ObjectsManager *managerObjects, std::function<void(std::string)> doLog) {
    this->managerObjects = managerObjects;
    this->funcDoLog = doLog;

    this->selectedObject = 0;
    this->selectedObjectLight = 0;
    this->selectedTabScene = -1;
    this->selectedTabGUICamera = -1;
    this->selectedTabGUIGrid = -1;
    this->selectedTabGUILight = -1;

    this->helperUI = new UIHelpers();
}

void DialogControlsGUI::render(bool* show, bool* isFrame) {
    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("GUI Controls", show, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0)))
        this->managerObjects->resetPropertiesSystem();
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
                if (ImGui::Selectable(ICON_FA_ASTERISK " General", this->selectedObject == i)) {
                    this->selectedObject = i;
                    this->selectedObjectLight = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 1: {
                ImGui::Indent();
                if (ImGui::Selectable(ICON_FA_VIDEO_CAMERA " Camera", this->selectedObject == i)) {
                    this->selectedObject = i;
                    this->selectedObjectLight = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 2: {
                ImGui::Indent();
                if (ImGui::Selectable(ICON_FA_BARS " Grid", this->selectedObject == i)) {
                    this->selectedObject = i;
                    this->selectedObjectLight = -1;
                }
                ImGui::Unindent();
                break;
            }
            case 3: {
                if (ImGui::TreeNode(ICON_FA_LIGHTBULB_O " Lights")) {
                    for (int j=0; j<(int)this->managerObjects->lightSources.size(); j++) {
                        ImGui::Bullet();
                        if (ImGui::Selectable(this->managerObjects->lightSources[j]->title.c_str(), this->selectedObjectLight == j)) {
                            this->selectedObjectLight = j;
                            this->selectedObject = 3;
                        }
                    }
                    ImGui::TreePop();
                }
                break;
            }
            case 4: {
                ImGui::Indent();
                if (ImGui::Selectable("Terrain", this->selectedObject == i)) {
                    this->selectedObject = i;
                    this->selectedObjectLight = -1;
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
    switch (this->selectedObject) {
        case 0: {
            this->helperUI->addControlsSlider("Field of view", 1, 1.0f, -180.0f, 180.0f, false, NULL, &this->managerObjects->Setting_FOV, true, isFrame);
            ImGui::Separator();

            ImGui::Text("Ratio"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("W & H");
            ImGui::SliderFloat("W##105", &this->managerObjects->Setting_RatioWidth, 0.0f, 5.0f);
            ImGui::SliderFloat("H##106", &this->managerObjects->Setting_RatioHeight, 0.0f, 5.0f);
            ImGui::Separator();

            ImGui::Text("Planes"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Far & Close");
            ImGui::SliderFloat("Far##107", &this->managerObjects->Setting_PlaneClose, 0.0f, 1.0f);
            ImGui::SliderFloat("Close##108", &this->managerObjects->Setting_PlaneFar, 0.0f, 100.0f);
            ImGui::Separator();

            this->helperUI->addControlColor4("Outline Color", &this->managerObjects->Setting_OutlineColor, &this->managerObjects->Setting_OutlineColorPickerOpen);
            this->helperUI->addControlsSlider("Outline Thickness", 2, 1.01f, 0.0f, 2.0f, false, NULL, &this->managerObjects->Setting_OutlineThickness, true, isFrame);
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
                    ImGui::Separator();
                    ImGui::Text("Eye");
                    this->helperUI->addControlsSliderSameLine("X", 1, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Eye.x, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 2, 1.0f, 100.0f, 10.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Eye.y, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 3, 1.0f, 0.0f, 90.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Eye.z, true, isFrame);
                    ImGui::Separator();
                    ImGui::Text("Center");
                    this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Center.x, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -10.0f, 10.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Center.y, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, 0.0f, 45.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Center.z, true, isFrame);
                    ImGui::Separator();
                    ImGui::Text("Up");
                    this->helperUI->addControlsSliderSameLine("X", 7, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Up.x, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 8, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Up.y, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 9, 1.0f, -1.0f, 1.0f, false, NULL, &this->managerObjects->camera->eyeSettings->View_Up.z, true, isFrame);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    this->helperUI->addControlsSliderSameLine("X", 10, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->camera->rotateX->animate, &this->managerObjects->camera->rotateX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 11, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->camera->rotateY->animate, &this->managerObjects->camera->rotateY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 12, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->camera->rotateZ->animate, &this->managerObjects->camera->rotateZ->point, true, isFrame);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    this->helperUI->addControlsSliderSameLine("X", 13, 0.05f, -2 * this->managerObjects->Setting_GridSize, 2 * this->managerObjects->Setting_GridSize, true, &this->managerObjects->camera->positionX->animate, &this->managerObjects->camera->positionX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 14, 0.05f, -2 * this->managerObjects->Setting_GridSize, 2 * this->managerObjects->Setting_GridSize, true, &this->managerObjects->camera->positionY->animate, &this->managerObjects->camera->positionY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 15, 0.05f, -2 * this->managerObjects->Setting_GridSize, 2 * this->managerObjects->Setting_GridSize, true, &this->managerObjects->camera->positionZ->animate, &this->managerObjects->camera->positionZ->point, true, isFrame);
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
                "\n" ICON_FA_ASTERISK,
                "\n" ICON_MD_PHOTO_SIZE_SELECT_SMALL,
                "\n" ICON_MD_3D_ROTATION,
                "\n" ICON_MD_OPEN_WITH,
            };
            const char* tabsLabelsGUIGrid[] = { "General", "Scale", "Rotate", "Translate" };
            const int numTabsGUIGrid = sizeof(tabsGUIGrid) / sizeof(tabsGUIGrid[0]);
            ImGui::TabLabels(numTabsGUIGrid, tabsGUIGrid, this->selectedTabGUIGrid, ImVec2(30.0, 30.0), tabsLabelsGUIGrid);
            ImGui::PopStyleColor(3);

            ImGui::Separator();

            switch (this->selectedTabGUIGrid) {
                case 0: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "General grid settings");
                    ImGui::Text("Grid size"); if (ImGui::IsItemHovered()) ImGui::SetTooltip("Squares");
                    ImGui::SliderInt("##109", &this->managerObjects->Setting_GridSize, 0, 100);
                    ImGui::Separator();
                    ImGui::Checkbox("Grid fixed with World", &this->managerObjects->Setting_FixedGridWorld);
                    ImGui::Checkbox("Show Grid", &this->managerObjects->grid->showGrid);
                    ImGui::Separator();
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->grid->scaleX->animate, &this->managerObjects->grid->scaleX->point, false, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->grid->scaleY->animate, &this->managerObjects->grid->scaleY->point, false, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->grid->scaleZ->animate, &this->managerObjects->grid->scaleZ->point, false, isFrame);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->grid->rotateX->animate, &this->managerObjects->grid->rotateX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->grid->rotateY->animate, &this->managerObjects->grid->rotateY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->grid->rotateZ->animate, &this->managerObjects->grid->rotateZ->point, true, isFrame);
                    break;
                }
                case 3: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->grid->positionX->animate, &this->managerObjects->grid->positionX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 8, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->grid->positionY->animate, &this->managerObjects->grid->positionY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 9, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->grid->positionZ->animate, &this->managerObjects->grid->positionZ->point, true, isFrame);
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
                    ImGui::Checkbox("Show Lamp", &this->managerObjects->lightSources[this->selectedObjectLight]->showLampObject);
                    ImGui::Checkbox("Show Direction", &this->managerObjects->lightSources[this->selectedObjectLight]->showLampDirection);
                    break;
                }
                case 1: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale object");
                    this->helperUI->addControlsSliderSameLine("X", 10, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleX->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 11, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleY->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 12, 0.05f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleZ->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->scaleZ->point, true, isFrame);
                    break;
                }
                case 2: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate object around axis");
                    this->helperUI->addControlsSliderSameLine("X", 13, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateX->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 14, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateY->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 15, 1.0f, -180.0f, 180.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateZ->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->rotateZ->point, true, isFrame);
                    break;
                }
                case 3: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move object by axis");
                    this->helperUI->addControlsSliderSameLine("X", 16, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->lightSources[this->selectedObjectLight]->positionX->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->positionX->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 17, 1.0f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->lightSources[this->selectedObjectLight]->positionY->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->positionY->point, true, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 18, 1.0f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &this->managerObjects->lightSources[this->selectedObjectLight]->positionZ->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->positionZ->point, true, isFrame);
                    break;
                }
                case 4: {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Light colors");

                    this->helperUI->addControlColor3("Ambient Color", &this->managerObjects->lightSources[this->selectedObjectLight]->ambient->color, &this->managerObjects->lightSources[this->selectedObjectLight]->ambient->colorPickerOpen);
                    this->helperUI->addControlsSlider("Ambient Strength", 19, 0.01f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->ambient->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->ambient->strength, true, isFrame);

                    this->helperUI->addControlColor3("Diffuse Color", &this->managerObjects->lightSources[this->selectedObjectLight]->diffuse->color, &this->managerObjects->lightSources[this->selectedObjectLight]->diffuse->animate);
                    this->helperUI->addControlsSlider("Diffuse Strength", 20, 0.01f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->diffuse->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->diffuse->strength, true, isFrame);

                    this->helperUI->addControlColor3("Specular Color", &this->managerObjects->lightSources[this->selectedObjectLight]->specular->color, &this->managerObjects->lightSources[this->selectedObjectLight]->specular->animate);
                    this->helperUI->addControlsSlider("Specular Strength", 21, 0.01f, 0.0f, 1.0f, true, &this->managerObjects->lightSources[this->selectedObjectLight]->specular->animate, &this->managerObjects->lightSources[this->selectedObjectLight]->specular->strength, true, isFrame);
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