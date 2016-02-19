//
//  DialogControlsModels.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogControlsModels.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/ui/components/Tabs.hpp"

void DialogControlsModels::init(ObjectsManager *managerObjects, std::function<void(std::string)> doLog) {
    this->managerObjects = managerObjects;
    this->funcDoLog = doLog;

    this->cmenu_deleteYn = false;
    this->cmenu_renameModel = false;
    this->selectedObject = 0;
    this->selectedTabScene = -1;
    this->selectedTabGUICamera = -1;
    this->selectedTabGUIGrid = -1;
    this->selectedTabGUILight = -1;

    this->helperUI = new UIHelpers();
    this->componentMaterialEditor = new MaterialEditor();
}

void DialogControlsModels::render(bool* show, bool* isFrame, std::vector<ModelFace*> * meshModelFaces) {
    ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(10, 28), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", show, ImGuiWindowFlags_ShowBorders);

    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(ImGui::GetWindowWidth() * 0.94f, 0))) {
        for (size_t i=0; i<meshModelFaces->size(); i++) {
            (*meshModelFaces)[i]->initModelProperties();
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::BeginChild("Scene Items", ImVec2(0, this->heightTopPanel), true);
    const char* scene_items[meshModelFaces->size()];
    for (size_t i=0; i<meshModelFaces->size(); i++) {
        scene_items[i] = (*meshModelFaces)[i]->oFace.ModelTitle.c_str();
    }

    // Scene Model
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.95f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));
    ImGui::ListBox("", &this->selectedObject, scene_items, IM_ARRAYSIZE(scene_items));
    ImGui::PopStyleVar(2);
    if (this->selectedObject > -1 && ImGui::BeginPopupContextItem("Actions")) {
        ImGui::MenuItem("Rename", NULL, &this->cmenu_renameModel);
        if (ImGui::MenuItem("Duplicate"))
            (*meshModelFaces).push_back((*meshModelFaces)[this->selectedObject]->clone((int)(*meshModelFaces).size() + 1));
        ImGui::MenuItem("Delete", NULL, &this->cmenu_deleteYn);
        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    if (this->cmenu_renameModel)
        this->contextModelRename(meshModelFaces);

    if (this->cmenu_deleteYn)
        this->contextModelDelete(meshModelFaces);
    ImGui::EndChild();

    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::InvisibleButton("splitter", ImVec2(-1, 8.0f));
    if (ImGui::IsItemActive())
        this->heightTopPanel += ImGui::GetIO().MouseDelta.y;
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(3);

    ImGui::BeginChild("Properties Pane", ImVec2(0,0), true);

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
    ModelFace *mmf = (*meshModelFaces)[this->selectedObject];
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
            ImGui::Checkbox("Cel Shading", &(*meshModelFaces)[this->selectedObject]->Setting_CelShading);
            // alpha
            ImGui::TextColored(ImVec4(1, 1, 1, (*meshModelFaces)[this->selectedObject]->Setting_Alpha), "Alpha Blending");
            ImGui::SliderFloat("", &(*meshModelFaces)[this->selectedObject]->Setting_Alpha, 0.0f, 1.0f);
            break;
        }
        case 1: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale Model");
            this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, 1.0f, true, &(*meshModelFaces)[this->selectedObject]->scaleX->animate, &(*meshModelFaces)[this->selectedObject]->scaleX->point, false, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, 1.0f, true, &(*meshModelFaces)[this->selectedObject]->scaleY->animate, &(*meshModelFaces)[this->selectedObject]->scaleY->point, false, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, 1.0f, true, &(*meshModelFaces)[this->selectedObject]->scaleZ->animate, &(*meshModelFaces)[this->selectedObject]->scaleZ->point, false, isFrame);
            break;
        }
        case 2: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate model around axis");
            this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[this->selectedObject]->rotateX->animate, &(*meshModelFaces)[this->selectedObject]->rotateX->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[this->selectedObject]->rotateY->animate, &(*meshModelFaces)[this->selectedObject]->rotateY->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[this->selectedObject]->rotateZ->animate, &(*meshModelFaces)[this->selectedObject]->rotateZ->point, true, isFrame);
            break;
        }
        case 3: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move model by axis");
            this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->positionX->animate, &(*meshModelFaces)[this->selectedObject]->positionX->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 8, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->positionY->animate, &(*meshModelFaces)[this->selectedObject]->positionY->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 9, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->positionZ->animate, &(*meshModelFaces)[this->selectedObject]->positionZ->point, true, isFrame);
            break;
        }
        case 4: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Displace model");
            this->helperUI->addControlsSliderSameLine("X", 10, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->displaceX->animate, &(*meshModelFaces)[this->selectedObject]->displaceX->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Y", 11, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->displaceY->animate, &(*meshModelFaces)[this->selectedObject]->displaceY->point, true, isFrame);
            this->helperUI->addControlsSliderSameLine("Z", 12, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->displaceZ->animate, &(*meshModelFaces)[this->selectedObject]->displaceZ->point, true, isFrame);
            break;
        }
        case 5: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Material of the model");
            if (ImGui::Button("Material Editor"))
                (*meshModelFaces)[this->selectedObject]->showMaterialEditor = true;
            this->helperUI->addControlsSlider("Refraction", 13, 0.05f, -10.0f, 10.0f, true, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialRefraction->animate, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialRefraction->point, true, isFrame);
            this->helperUI->addControlsSlider("Specular Exponent", 14, 10.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialSpecularExp->animate, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialSpecularExp->point, true, isFrame);
            this->helperUI->addControlColor3("Ambient Color", &(*meshModelFaces)[this->selectedObject]->materialAmbient->color, &(*meshModelFaces)[this->selectedObject]->materialAmbient->colorPickerOpen);
            this->helperUI->addControlColor3("Diffuse Color", &(*meshModelFaces)[this->selectedObject]->materialDiffuse->color, &(*meshModelFaces)[this->selectedObject]->materialDiffuse->colorPickerOpen);
            this->helperUI->addControlColor3("Specular Color", &(*meshModelFaces)[this->selectedObject]->materialSpecular->color, &(*meshModelFaces)[this->selectedObject]->materialSpecular->colorPickerOpen);
            this->helperUI->addControlColor3("Emission Color", &(*meshModelFaces)[this->selectedObject]->materialEmission->color, &(*meshModelFaces)[this->selectedObject]->materialEmission->colorPickerOpen);
            break;
        }
        case 6: {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Illumination type");
            const char* illum_models_items[] = {
                "[0] Color on and Ambient off",  // "Shaderless" option in Blender, under "Shading" in Material tab
                "[1] Color on and Ambient on",
                "[2] Highlight on",
                "[3] Reflection on and Raytrace on",
                "[4] Transparency: Glass on\n    Reflection: Raytrace on",
                "[5] Reflection: Fresnel on\n    Raytrace on",
                "[6] Transparency: Refraction on\n    Reflection: Fresnel off\n    Raytrace on",
                "[7] Transparency: Refraction on\n    Reflection: Fresnel on\n    Raytrace on",
                "[8] Reflection on\n    Raytrace off",
                "[9] Transparency: Glass on\n    Reflection: Raytrace off",
                "[10] Casts shadows onto invisible surfaces"
            };
            ImGui::Combo("##987", &(*meshModelFaces)[this->selectedObject]->materialIlluminationModel, illum_models_items, IM_ARRAYSIZE(illum_models_items));
            break;
        }
        default:
            break;
    }
    ImGui::PopItemWidth();

    ImGui::EndChild();

    if ((*meshModelFaces)[this->selectedObject]->showMaterialEditor)
        this->componentMaterialEditor->draw(&(*meshModelFaces)[this->selectedObject]->showMaterialEditor);

    ImGui::End();
}


void DialogControlsModels::contextModelRename(std::vector<ModelFace*> * meshModelFaces) {
    ImGui::OpenPopup("Rename");

    ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Type the new model name:");
    ImGui::Text("(%s)", (*meshModelFaces)[this->selectedObject]->oFace.ModelTitle.c_str());

    if (this->guiModelRenameText[0] == '\0')
        strcpy(this->guiModelRenameText, (*meshModelFaces)[this->selectedObject]->oFace.ModelTitle.c_str());
    ImGui::InputText("", this->guiModelRenameText, sizeof(this->guiModelRenameText));

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*meshModelFaces)[this->selectedObject]->oFace.ModelTitle = std::string(this->guiModelRenameText);
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

void DialogControlsModels::contextModelDelete(std::vector<ModelFace*> * meshModelFaces) {
    ImGui::OpenPopup("Delete?");

    ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Are you sure you want to delete this model?\n");
    ImGui::Text("(%s)", (*meshModelFaces)[this->selectedObject]->oFace.ModelTitle.c_str());

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*meshModelFaces).erase((*meshModelFaces).begin() + this->selectedObject);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) { ImGui::CloseCurrentPopup(); this->cmenu_deleteYn = false; }

    ImGui::EndPopup();
}

