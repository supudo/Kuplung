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
#include <boost/filesystem.hpp>
#include "kuplung/utilities/stb/stb_image.h"

void DialogControlsModels::init(SDL_Window* sdlWindow, ObjectsManager *managerObjects, std::function<void(ShapeType)> addShape) {
    this->sdlWindow = sdlWindow;
    this->managerObjects = managerObjects;
    this->funcAddShape = addShape;

    this->cmenu_deleteYn = false;
    this->cmenu_renameModel = false;

    this->showTextureWindow_Ambient = false;
    this->showTexture_Ambient = false;
    this->showTextureWindow_Diffuse = false;
    this->showTexture_Diffuse = false;
    this->showTextureWindow_Dissolve = false;
    this->showTexture_Dissolve = false;
    this->showTextureWindow_Bump = false;
    this->showTexture_Bump = false;
    this->showTextureWindow_Specular = false;
    this->showTexture_Specular = false;
    this->showTextureWindow_SpecularExp = false;
    this->showTexture_SpecularExp = false;

    this->textureAmbient_Width = this->textureAmbient_Height = this->textureDiffuse_Width = this->textureDiffuse_Height = 0;
    this->textureDissolve_Width = this->textureDissolve_Height = this->textureBump_Width = this->textureBump_Height = 0;
    this->textureSpecular_Width = this->textureSpecular_Height = this->textureSpecularExp_Width = this->textureSpecularExp_Height = 0;

    this->selectedObject = 0;
    this->selectedTabScene = -1;
    this->selectedTabGUICamera = -1;
    this->selectedTabGUIGrid = -1;
    this->selectedTabGUILight = -1;
    this->selectedTabPanel = 1;

    this->helperUI = new UIHelpers();
    this->componentMaterialEditor = new MaterialEditor();
    this->componentMaterialEditor->init();
}

void DialogControlsModels::showTextureImage(ModelFace* mmf, int type, std::string title, bool* showWindow, bool* genTexture, GLuint* vboBuffer, int* width, int* height) {
    int wWidth, wHeight, tWidth, tHeight, posX, posY;
    SDL_GetWindowSize(this->sdlWindow, &wWidth, &wHeight);

    std::string img = "";
    if (type == 0)
        img = mmf->oFace.faceMaterial.textures_ambient.image;
    else if (type == 1)
        img = mmf->oFace.faceMaterial.textures_diffuse.image;
    else if (type == 2)
        img = mmf->oFace.faceMaterial.textures_dissolve.image;
    else if (type == 3)
        img = mmf->oFace.faceMaterial.textures_bump.image;
    else if (type == 4)
        img = mmf->oFace.faceMaterial.textures_displacement.image;
    else if (type == 5)
        img = mmf->oFace.faceMaterial.textures_specular.image;
    else
        img = mmf->oFace.faceMaterial.textures_specularExp.image;

    if (*genTexture)
        this->createTextureBuffer(img, vboBuffer, width, height);

    tWidth = *width + 20;
    if (tWidth > wWidth)
        tWidth = wWidth - 20;

    tHeight = *height + 20;
    if (tHeight > wHeight)
        tHeight = wHeight - 40;

    ImGuiWindow *gw = ImGui::GetCurrentWindow();
    posX = gw->Pos.x + gw->Rect().GetWidth()  + 20;
    posY = 20;

    ImGui::SetNextWindowSize(ImVec2(tWidth, tHeight), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiSetCond_FirstUseEver);

    title = title + " Texture";
    ImGui::Begin(title.c_str(), showWindow, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::Text("Image: %s", img.c_str());
    ImGui::Text("Image dimensions: %i x %i", *width, *height);

    ImGui::Separator();

    ImGui::Image((ImTextureID)(intptr_t)*vboBuffer, ImVec2(*width, *height));

    ImGui::End();
    *genTexture = false;
}

void DialogControlsModels::createTextureBuffer(std::string imageFile, GLuint* vboBuffer, int* width, int* height) {
    if (!boost::filesystem::exists(imageFile))
        imageFile = Settings::Instance()->currentFolder + "/" + imageFile;
    int tChannels;
    unsigned char* tPixels = stbi_load(imageFile.c_str(), width, height, &tChannels, 0);
    if (!tPixels)
        Settings::Instance()->funcDoLog("Can't load bump texture image - " + imageFile + " with error - " + std::string(stbi_failure_reason()));
    else {
        glGenTextures(1, vboBuffer);
        glBindTexture(GL_TEXTURE_2D, *vboBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GLint textureFormat = 0;
        switch (tChannels) {
            case 1:
                textureFormat = GL_LUMINANCE;
                break;
            case 2:
                textureFormat = GL_LUMINANCE_ALPHA;
                break;
            case 3:
                textureFormat = GL_RGB;
                break;
            case 4:
                textureFormat = GL_RGBA;
                break;
            default:
                textureFormat = GL_RGB;
                break;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, *width, *height, 0, textureFormat, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tPixels);
    }
}

void DialogControlsModels::showTextureLine(std::string chkLabel, std::string title, bool* useTexture, bool* showWindow, bool* loadTexture, std::string image) {
    ImGui::Checkbox(chkLabel.c_str(), useTexture);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", ("Show/Hide " + title + " Texture").c_str());
    ImGui::SameLine();
    std::string btnLabel = ICON_FA_EYE;
    btnLabel += chkLabel;
    if (ImGui::Button(btnLabel.c_str())) {
        *showWindow = !*showWindow;
        *loadTexture = true;
    }
    ImGui::SameLine();
    ImGui::Text("%s: %s", title.c_str(), image.c_str());
}

void DialogControlsModels::render(bool* show, bool* isFrame, std::vector<ModelFace*> * meshModelFaces, std::vector<objScene> *scenes) {
    ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(10, 28), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", show, ImGuiWindowFlags_ShowBorders);

    ImGui::BeginChild("tabs_list", ImVec2(-1, this->panelHeight_Tabs));
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));

    const char* tabsPanels[] = {
        "\n  " ICON_MD_BUILD "  ",
        "\n  " ICON_MD_ADD "  "
    };
    const char* tabsPanelsLabels[] = { "Models", "Create" };
    const int numTabsPanels = sizeof(tabsPanels) / sizeof(tabsPanels[0]);
    ImGui::TabLabels(numTabsPanels, tabsPanels, this->selectedTabPanel, ImVec2(50.0, 30.0), tabsPanelsLabels);

    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::BeginGroup();

    switch (this->selectedTabPanel) {
        case 0:
            if (meshModelFaces != NULL)
                this->drawModels(isFrame, meshModelFaces, scenes);
            else
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "No models in the current scene.");
            break;
        case 1:
            this->drawCreate();
            break;
        default:
            break;
    }

    ImGui::EndGroup();

    ImGui::End();
}

bool DialogControlsModels::isRootNode(std::vector<const char*> scenes_items, int selectedItem) {
    std::string haystack = std::string(scenes_items[selectedItem]);
    std::string needle = ICON_FA_LEMON_O " ";
    if (haystack.compare(0, needle.length(), needle) == 0)
        return true;
    else
        return false;
}

int DialogControlsModels::getConvertedSelectedObject(std::vector<const char*> scene_items) {
    int convertedSelectedObject = 0;
    for (int i=0; i<this->selectedObject; i++) {
        if (!this->isRootNode(scene_items, i))
            convertedSelectedObject += 1;
    }
    return convertedSelectedObject;
}

void DialogControlsModels::drawModels(bool* isFrame, std::vector<ModelFace*> * meshModelFaces, std::vector<objScene> *scenes) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(-1, 0))) {
        for (size_t i=0; i<meshModelFaces->size(); i++) {
            (*meshModelFaces)[i]->initModelProperties();
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::BeginChild("Scene Items", ImVec2(0, this->heightTopPanel), true);

    // Scene Model
    ImGui::PushItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(1, 0, 0, 1));

    std::vector<const char*> scene_items;

    std::vector<std::string> s_s_items;
    for (size_t i=0; i<scenes->size(); i++) {
        s_s_items.push_back(ICON_FA_LEMON_O " " + (*scenes)[i].objFile);
        for (size_t j=0; j<(*scenes)[i].models.size(); j++) {
            objModel m = (*scenes)[i].models[j];
            for (size_t k=0; k<m.faces.size(); k++) {
                s_s_items.push_back("    " ICON_FA_CUBE " " + m.faces[k].ModelTitle);
            }
        }
    }

    for (size_t i=0; i<s_s_items.size(); i++) {
        scene_items.push_back(s_s_items[i].c_str());
    }
    ImGui::ListBox("", &this->selectedObject, &scene_items[0], (int)scene_items.size());


//    for (size_t i=0; i<meshModelFaces->size(); i++) {
//        scene_items.push_back((ICON_FA_CUBE " " + (*meshModelFaces)[i]->oFace.ModelTitle).c_str());
//    }
//    ImGui::ListBox("", &this->selectedObject, &scene_items[0], (int)meshModelFaces->size());
    ImGui::PopStyleColor(1);
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
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterModels", ImVec2(-1, 8.0f));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive())
        this->heightTopPanel += ImGui::GetIO().MouseDelta.y;
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(3);

    ImGui::BeginChild("Properties Pane", ImVec2(0,0), false);

    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.75f);
    if (this->isRootNode(scene_items, this->selectedObject)) {
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Group:"); ImGui::SameLine(); ImGui::Text("%s", scene_items[this->selectedObject]);
    }
    else {
        int convertedSelectedObject = this->getConvertedSelectedObject(scene_items);
        ModelFace *mmf = (*meshModelFaces)[convertedSelectedObject];
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "OBJ File:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.objFile.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Model:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.ModelTitle.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Material:"); ImGui::SameLine(); ImGui::Text("%s", mmf->oFace.materialID.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Vertices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.verticesCount);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Normals:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.normalsCount);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Indices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->oFace.indicesCount);

        if (mmf->oFace.faceMaterial.textures_ambient.image != "" ||
            mmf->oFace.faceMaterial.textures_diffuse.image != "" ||
            mmf->oFace.faceMaterial.textures_dissolve.image != "" ||
            mmf->oFace.faceMaterial.textures_specular.image != "" ||
            mmf->oFace.faceMaterial.textures_specularExp.image != "") {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Textures");
        }
        if (mmf->oFace.faceMaterial.textures_ambient.image != "") {
            this->showTextureLine("##001",
                                  "Ambient",
                                  &mmf->oFace.faceMaterial.textures_ambient.useTexture,
                                  &this->showTextureWindow_Ambient,
                                  &this->showTexture_Ambient,
                                  mmf->oFace.faceMaterial.textures_ambient.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_diffuse.image != "") {
            this->showTextureLine("##002",
                                  "Diffuse",
                                  &mmf->oFace.faceMaterial.textures_diffuse.useTexture,
                                  &this->showTextureWindow_Diffuse,
                                  &this->showTexture_Diffuse,
                                  mmf->oFace.faceMaterial.textures_diffuse.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_dissolve.image != "") {
            this->showTextureLine("##003",
                                  "Dissolve",
                                  &mmf->oFace.faceMaterial.textures_dissolve.useTexture,
                                  &this->showTextureWindow_Dissolve,
                                  &this->showTexture_Dissolve,
                                  mmf->oFace.faceMaterial.textures_dissolve.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_bump.image != "") {
            this->showTextureLine("##004",
                                  "Bump",
                                  &mmf->oFace.faceMaterial.textures_bump.useTexture,
                                  &this->showTextureWindow_Bump,
                                  &this->showTexture_Bump,
                                  mmf->oFace.faceMaterial.textures_bump.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_displacement.image != "") {
            this->showTextureLine("##005",
                                  "Height",
                                  &mmf->oFace.faceMaterial.textures_displacement.useTexture,
                                  &this->showTextureWindow_Displacement,
                                  &this->showTexture_Displacement,
                                  mmf->oFace.faceMaterial.textures_displacement.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_specular.image != "") {
            this->showTextureLine("##006",
                                  "Specular",
                                  &mmf->oFace.faceMaterial.textures_specular.useTexture,
                                  &this->showTextureWindow_Specular,
                                  &this->showTexture_Specular,
                                  mmf->oFace.faceMaterial.textures_specular.image.c_str());
        }
        if (mmf->oFace.faceMaterial.textures_specularExp.image != "") {
            this->showTextureLine("##007",
                                  "SpecularExp",
                                  &mmf->oFace.faceMaterial.textures_specularExp.useTexture,
                                  &this->showTextureWindow_SpecularExp,
                                  &this->showTexture_SpecularExp,
                                  mmf->oFace.faceMaterial.textures_specularExp.image.c_str());
        }

        if (this->showTextureWindow_Ambient)
            this->showTextureImage(mmf, 0, "Ambient", &this->showTextureWindow_Ambient, &this->showTexture_Ambient, &this->vboTextureAmbient, &this->textureAmbient_Width, &this->textureAmbient_Height);

        if (this->showTextureWindow_Diffuse)
            this->showTextureImage(mmf, 1, "Diffuse", &this->showTextureWindow_Diffuse, &this->showTexture_Diffuse, &this->vboTextureDiffuse, &this->textureDiffuse_Width, &this->textureDiffuse_Height);

        if (this->showTextureWindow_Dissolve)
            this->showTextureImage(mmf, 2, "Dissolve", &this->showTextureWindow_Dissolve, &this->showTexture_Dissolve, &this->vboTextureDissolve, &this->textureDissolve_Width, &this->textureDissolve_Height);

        if (this->showTextureWindow_Bump)
            this->showTextureImage(mmf, 3, "Bump", &this->showTextureWindow_Bump, &this->showTexture_Bump, &this->vboTextureBump, &this->textureBump_Width, &this->textureBump_Height);

        if (this->showTextureWindow_Displacement)
            this->showTextureImage(mmf, 4, "Height", &this->showTextureWindow_Displacement, &this->showTexture_Displacement, &this->vboTextureDisplacement, &this->textureDisplacement_Width, &this->textureDisplacement_Height);

        if (this->showTextureWindow_Specular)
            this->showTextureImage(mmf, 5, "Specular", &this->showTextureWindow_Specular, &this->showTexture_Specular, &this->vboTextureSpecular, &this->textureSpecular_Width, &this->textureSpecular_Height);

        if (this->showTextureWindow_SpecularExp)
            this->showTextureImage(mmf, 6, "SpecularExp", &this->showTextureWindow_SpecularExp, &this->showTexture_SpecularExp, &this->vboTextureSpecularExp, &this->textureSpecularExp_Width, &this->textureSpecularExp_Height);

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
            "\n" ICON_MD_BLUR_ON,
            "\n" ICON_MD_LIGHTBULB_OUTLINE,
        };
        const char* tabsLabelsScene[] = { "General", "Scale", "Rotate", "Translate", "Displace", "Material", "Effects", "Illumination" };
        const int numTabsScene = sizeof(tabsScene) / sizeof(tabsScene[0]);
        ImGui::TabLabels(numTabsScene, tabsScene, this->selectedTabScene, ImVec2(30.0, 30.0), tabsLabelsScene);
        ImGui::PopStyleColor(3);

        ImGui::Separator();

        switch (this->selectedTabScene) {
            case 0: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Properties");
                // cel shading
                ImGui::Checkbox("Cel Shading", &(*meshModelFaces)[convertedSelectedObject]->Setting_CelShading);
                ImGui::Checkbox("Wireframe", &(*meshModelFaces)[convertedSelectedObject]->Setting_Wireframe);
                // alpha
                ImGui::TextColored(ImVec4(1, 1, 1, (*meshModelFaces)[convertedSelectedObject]->Setting_Alpha), "Alpha Blending");
                ImGui::SliderFloat("", &(*meshModelFaces)[convertedSelectedObject]->Setting_Alpha, 0.0f, 1.0f);
                break;
            }
            case 1: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale Model");
                this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[convertedSelectedObject]->scaleX->animate, &(*meshModelFaces)[convertedSelectedObject]->scaleX->point, false, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[convertedSelectedObject]->scaleY->animate, &(*meshModelFaces)[convertedSelectedObject]->scaleY->point, false, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[convertedSelectedObject]->scaleZ->animate, &(*meshModelFaces)[convertedSelectedObject]->scaleZ->point, false, isFrame);
                break;
            }
            case 2: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate model around axis");
                this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[convertedSelectedObject]->rotateX->animate, &(*meshModelFaces)[convertedSelectedObject]->rotateX->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[convertedSelectedObject]->rotateY->animate, &(*meshModelFaces)[convertedSelectedObject]->rotateY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &(*meshModelFaces)[convertedSelectedObject]->rotateZ->animate, &(*meshModelFaces)[convertedSelectedObject]->rotateZ->point, true, isFrame);
                break;
            }
            case 3: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move model by axis");
                this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->positionX->animate, &(*meshModelFaces)[convertedSelectedObject]->positionX->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 8, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->positionY->animate, &(*meshModelFaces)[convertedSelectedObject]->positionY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 9, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->positionZ->animate, &(*meshModelFaces)[convertedSelectedObject]->positionZ->point, true, isFrame);
                break;
            }
            case 4: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Displace model");
                this->helperUI->addControlsSliderSameLine("X", 10, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->displaceX->animate, &(*meshModelFaces)[convertedSelectedObject]->displaceX->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 11, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->displaceY->animate, &(*meshModelFaces)[convertedSelectedObject]->displaceY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 12, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[convertedSelectedObject]->displaceZ->animate, &(*meshModelFaces)[convertedSelectedObject]->displaceZ->point, true, isFrame);
                break;
            }
            case 5: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Material of the model");
                if (ImGui::Button("Material Editor"))
                    (*meshModelFaces)[convertedSelectedObject]->showMaterialEditor = true;
                ImGui::Checkbox("Parallax Mapping", &(*meshModelFaces)[convertedSelectedObject]->Setting_ParallaxMapping);
                ImGui::Separator();
                ImGui::Checkbox("Use Tessellation", &(*meshModelFaces)[convertedSelectedObject]->Setting_UseTessellation);
                if ((*meshModelFaces)[convertedSelectedObject]->Setting_UseTessellation) {
                    ImGui::Checkbox("Culling", &(*meshModelFaces)[convertedSelectedObject]->Setting_UseCullFace);
                    this->helperUI->addControlsIntegerSlider("Subdivision", 24, 0, 100, &(*meshModelFaces)[convertedSelectedObject]->Setting_TessellationSubdivision);
                    ImGui::Separator();
                    if (mmf->oFace.faceMaterial.textures_displacement.useTexture) {
                        this->helperUI->addControlsSlider("Displacement", 15, 0.05f, 0.0f, 10.0f, true, &(*meshModelFaces)[convertedSelectedObject]->displacementHeightScale->animate, &(*meshModelFaces)[convertedSelectedObject]->displacementHeightScale->point, false, isFrame);
                        ImGui::Separator();
                    }
                }
                else
                    ImGui::Separator();
                this->helperUI->addControlsSlider("Refraction", 13, 0.05f, -10.0f, 10.0f, true, &(*meshModelFaces)[convertedSelectedObject]->Setting_MaterialRefraction->animate, &(*meshModelFaces)[convertedSelectedObject]->Setting_MaterialRefraction->point, true, isFrame);
                this->helperUI->addControlsSlider("Specular Exponent", 14, 10.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[convertedSelectedObject]->Setting_MaterialSpecularExp->animate, &(*meshModelFaces)[convertedSelectedObject]->Setting_MaterialSpecularExp->point, true, isFrame);
                ImGui::Separator();
                this->helperUI->addControlColor3("Ambient Color", &(*meshModelFaces)[convertedSelectedObject]->materialAmbient->color, &(*meshModelFaces)[convertedSelectedObject]->materialAmbient->colorPickerOpen);
                this->helperUI->addControlColor3("Diffuse Color", &(*meshModelFaces)[convertedSelectedObject]->materialDiffuse->color, &(*meshModelFaces)[convertedSelectedObject]->materialDiffuse->colorPickerOpen);
                this->helperUI->addControlColor3("Specular Color", &(*meshModelFaces)[convertedSelectedObject]->materialSpecular->color, &(*meshModelFaces)[convertedSelectedObject]->materialSpecular->colorPickerOpen);
                this->helperUI->addControlColor3("Emission Color", &(*meshModelFaces)[convertedSelectedObject]->materialEmission->color, &(*meshModelFaces)[convertedSelectedObject]->materialEmission->colorPickerOpen);
                break;
            }
            case 6: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Effects");

                ImGui::Text("Gaussian Blur");
                const char* gb_items[] = {"No Blur", "Horizontal", "Vertical"};
                ImGui::Combo("Mode##228", &(*meshModelFaces)[convertedSelectedObject]->Effect_GBlur_Mode, gb_items, IM_ARRAYSIZE(gb_items));
                this->helperUI->addControlsSlider("Radius", 16, 0.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[convertedSelectedObject]->Effect_GBlur_Radius->animate, &(*meshModelFaces)[convertedSelectedObject]->Effect_GBlur_Radius->point, true, isFrame);
                this->helperUI->addControlsSlider("Width", 17, 0.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[convertedSelectedObject]->Effect_GBlur_Width->animate, &(*meshModelFaces)[convertedSelectedObject]->Effect_GBlur_Width->point, true, isFrame);
                ImGui::Separator();

    //            ImGui::Text("Bloom");
    //            ImGui::Checkbox("Apply Bloom", &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_doBloom);
    //            this->helperUI->addControlsSlider("Ambient", 18, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_WeightA, false, isFrame);
    //            this->helperUI->addControlsSlider("Specular", 19, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_WeightB, false, isFrame);
    //            this->helperUI->addControlsSlider("Specular Exp", 20, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_WeightC, false, isFrame);
    //            this->helperUI->addControlsSlider("Dissolve", 21, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_WeightD, false, isFrame);
    //            this->helperUI->addControlsSlider("Vignette", 22, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_Vignette, false, isFrame);
    //            this->helperUI->addControlsSlider("Vignette Attenuation", 23, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[convertedSelectedObject]->Effect_Bloom_VignetteAtt, false, isFrame);
    //            ImGui::Separator();
                break;
            }
            case 7: {
                // https://en.wikipedia.org/wiki/List_of_common_shading_algorithms
                // https://renderman.pixar.com/view/cook-torrance-shader
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
                ImGui::Combo("##987", &(*meshModelFaces)[convertedSelectedObject]->materialIlluminationModel, illum_models_items, IM_ARRAYSIZE(illum_models_items));
                break;
            }
            default:
                break;
        }

        if (meshModelFaces != NULL && (*meshModelFaces)[convertedSelectedObject]->showMaterialEditor)
            this->componentMaterialEditor->draw((*meshModelFaces)[convertedSelectedObject], &(*meshModelFaces)[convertedSelectedObject]->showMaterialEditor);
    }

    ImGui::PopItemWidth();

    ImGui::EndChild();
}

void DialogControlsModels::drawCreate() {
    // https://developer.apple.com/library/mac/samplecode/OpenGLFilterBasicsCocoa/Listings/Sources_Models_Torus_Classes_OpenGLTorusGenerator_cpp.html

    if (ImGui::Button("Cone", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Cone);
    if (ImGui::Button("Cube", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Cube);
    if (ImGui::Button("Cylinder", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Cylinder);
    if (ImGui::Button("Grid", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Grid);
    if (ImGui::Button("Ico Sphere", ImVec2(-1, 0))) this->funcAddShape(ShapeType_IcoSphere);
    if (ImGui::Button("Plane", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Plane);
    if (ImGui::Button("Torus", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Torus);
    if (ImGui::Button("Tube", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Tube);
    if (ImGui::Button("UV Sphere", ImVec2(-1, 0))) this->funcAddShape(ShapeType_UVSphere);
    if (ImGui::Button("Monkey Head", ImVec2(-1, 0))) this->funcAddShape(ShapeType_MonkeyHead);

    ImGui::Separator();

    ImGui::Button("Directional (Sun)", ImVec2(-1, 0));
    ImGui::Button("Point (Light bulb)", ImVec2(-1, 0));
    ImGui::Button("Spot (Flashlight)", ImVec2(-1, 0));
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

