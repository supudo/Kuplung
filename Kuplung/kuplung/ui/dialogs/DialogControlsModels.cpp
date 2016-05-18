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

void DialogControlsModels::init(SDL_Window* sdlWindow, ObjectsManager *managerObjects, std::function<void(ShapeType)> addShape, std::function<void(LightSourceType)> addLight, std::function<void(int)> deleteModel) {
    this->sdlWindow = sdlWindow;
    this->managerObjects = managerObjects;
    this->funcAddShape = addShape;
    this->funcAddLight = addLight;
    this->funcDeleteModel = deleteModel;

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
    this->showUVEditor = false;

    this->TextureImage = "";
    this->TextureFilename = "";

    this->textureAmbient_Width = this->textureAmbient_Height = this->textureDiffuse_Width = this->textureDiffuse_Height = 0;
    this->textureDissolve_Width = this->textureDissolve_Height = this->textureBump_Width = this->textureBump_Height = 0;
    this->textureSpecular_Width = this->textureSpecular_Height = this->textureSpecularExp_Width = this->textureSpecularExp_Height = 0;

    this->selectedObject = -1;
    this->selectedTabScene = -1;
    this->selectedTabGUICamera = -1;
    this->selectedTabGUIGrid = -1;
    this->selectedTabGUILight = -1;
    this->selectedTabPanel = 1;

    this->helperUI = new UIHelpers();
    this->componentMaterialEditor = new MaterialEditor();
    this->componentMaterialEditor->init();

    this->componentUVEditor = new UVEditor();
    this->componentUVEditor->init(50, 50, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height);
}

void DialogControlsModels::showTextureImage(ModelFace* mmf, int type, std::string title, bool* showWindow, bool* genTexture, GLuint* vboBuffer, int* width, int* height) {
    int wWidth, wHeight, tWidth, tHeight, posX, posY;
    SDL_GetWindowSize(this->sdlWindow, &wWidth, &wHeight);

    std::string img = "";
    if (type == 0)
        img = mmf->meshModel.ModelMaterial.TextureAmbient.Image;
    else if (type == 1)
        img = mmf->meshModel.ModelMaterial.TextureDiffuse.Image;
    else if (type == 2)
        img = mmf->meshModel.ModelMaterial.TextureDissolve.Image;
    else if (type == 3)
        img = mmf->meshModel.ModelMaterial.TextureBump.Image;
    else if (type == 4)
        img = mmf->meshModel.ModelMaterial.TextureDisplacement.Image;
    else if (type == 5)
        img = mmf->meshModel.ModelMaterial.TextureSpecular.Image;
    else
        img = mmf->meshModel.ModelMaterial.TextureSpecularExp.Image;

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
        Settings::Instance()->funcDoLog("Can't load texture image - " + imageFile + " with error - " + std::string(stbi_failure_reason()));
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

void DialogControlsModels::render(bool* show, bool* isFrame, std::vector<ModelFace*> * meshModelFaces, int * sceneSelectedModelObject) {
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
                this->drawModels(isFrame, meshModelFaces);
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

    if (this->selectedObject > -1 && meshModelFaces != NULL && (int)(*meshModelFaces).size() > this->selectedObject && (*meshModelFaces)[this->selectedObject]->showMaterialEditor)
        this->componentMaterialEditor->draw(this->selectedObject, (*meshModelFaces)[this->selectedObject], &(*meshModelFaces)[this->selectedObject]->showMaterialEditor);

    *sceneSelectedModelObject = this->selectedObject;

    ImGui::End();
}

void DialogControlsModels::showTextureAdd(MaterialTextureType mtType) {
    std::string btnLabel = ICON_FA_EYE " Add Texture";
    btnLabel += " " + Kuplung_getTextureName(mtType);
    if (ImGui::Button(btnLabel.c_str())) {
        this->showUVEditor = true;
        this->componentUVEditor->setModel((*this->meshModelFaces)[this->selectedObject], mtType, "",
                std::bind(&DialogControlsModels::processTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    if (this->showUVEditor)
        this->componentUVEditor->draw("UV Editor", &this->showUVEditor);
}

void DialogControlsModels::processTexture(ModelFace *mmf, MaterialTextureType texType, std::string texturePath, std::vector<glm::vec2> textureCoordinates) {
    this->showUVEditor = false;
//    (*this->meshModelFaces)[this->selectedObject]->meshModel.texture_coordinates = textureCoordinates;
//    (*this->meshModelFaces)[this->selectedObject]->meshModel.countTextureCoordinates = (int)textureCoordinates.size();
//    switch (texType) {
//        case MaterialTextureType_Ambient:
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureAmbient.UseTexture = true;
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureAmbient.Image = texturePath;
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureDiffuse.Filename = texturePath;
//            break;
//        case MaterialTextureType_Diffuse:
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureDiffuse.UseTexture = true;
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureDiffuse.Image = texturePath;
//            (*this->meshModelFaces)[this->selectedObject]->meshModel.ModelMaterial.TextureDiffuse.Filename = texturePath;
//            break;
//        default:
//            break;
//    }
    (*this->meshModelFaces)[this->selectedObject] = mmf;
    (*this->meshModelFaces)[this->selectedObject]->initBuffersAgain = true;
}

void DialogControlsModels::drawModels(bool* isFrame, std::vector<ModelFace*> * meshModelFaces) {
    this->meshModelFaces = meshModelFaces;
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

    std::vector<const char*> scene_items;
    for (size_t i=0; i<meshModelFaces->size(); i++) {
        scene_items.push_back((*meshModelFaces)[i]->meshModel.ModelTitle.c_str());
    }

    // Scene Model
    ImGui::PushItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(1, 0, 0, 1));
    ImGui::ListBox("", &this->selectedObject, &scene_items[0], (int)meshModelFaces->size());
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

    if (this->selectedObject > -1) {
        ModelFace *mmf = (*meshModelFaces)[this->selectedObject];
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "OBJ File:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.File.title.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Model:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.ModelTitle.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Material:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.MaterialTitle.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Vertices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countVertices);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Normals:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countNormals);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Indices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countIndices);

        if (mmf->meshModel.ModelMaterial.TextureAmbient.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureDiffuse.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureBump.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureDisplacement.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureDissolve.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureSpecular.Image != "" ||
            mmf->meshModel.ModelMaterial.TextureSpecularExp.Image != "") {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Textures");
        }

        if (mmf->meshModel.ModelMaterial.TextureAmbient.Image != "") {
            this->showTextureLine("##001",
                                  "Ambient",
                                  &mmf->meshModel.ModelMaterial.TextureAmbient.UseTexture,
                                  &this->showTextureWindow_Ambient,
                                  &this->showTexture_Ambient,
                                  mmf->meshModel.ModelMaterial.TextureAmbient.Image.c_str());
        }
        if (mmf->meshModel.ModelMaterial.TextureDiffuse.Image != "") {
            this->showTextureLine("##002",
                                  "Diffuse",
                                  &mmf->meshModel.ModelMaterial.TextureDiffuse.UseTexture,
                                  &this->showTextureWindow_Diffuse,
                                  &this->showTexture_Diffuse,
                                  mmf->meshModel.ModelMaterial.TextureDiffuse.Image.c_str());
        }
        // TODO: add texture coordinates - unwrap
        else
            this->showTextureAdd(MaterialTextureType_Diffuse);
        if (mmf->meshModel.ModelMaterial.TextureDissolve.Image != "") {
            this->showTextureLine("##003",
                                  "Dissolve",
                                  &mmf->meshModel.ModelMaterial.TextureDissolve.UseTexture,
                                  &this->showTextureWindow_Dissolve,
                                  &this->showTexture_Dissolve,
                                  mmf->meshModel.ModelMaterial.TextureDissolve.Image.c_str());
        }
        if (mmf->meshModel.ModelMaterial.TextureBump.Image != "") {
            this->showTextureLine("##004",
                                  "Bump",
                                  &mmf->meshModel.ModelMaterial.TextureBump.UseTexture,
                                  &this->showTextureWindow_Bump,
                                  &this->showTexture_Bump,
                                  mmf->meshModel.ModelMaterial.TextureBump.Image.c_str());
        }
        if (mmf->meshModel.ModelMaterial.TextureDisplacement.Image != "") {
            this->showTextureLine("##005",
                                  "Height",
                                  &mmf->meshModel.ModelMaterial.TextureDisplacement.UseTexture,
                                  &this->showTextureWindow_Displacement,
                                  &this->showTexture_Displacement,
                                  mmf->meshModel.ModelMaterial.TextureDisplacement.Image.c_str());
        }
        if (mmf->meshModel.ModelMaterial.TextureSpecular.Image != "") {
            this->showTextureLine("##006",
                                  "Specular",
                                  &mmf->meshModel.ModelMaterial.TextureSpecular.UseTexture,
                                  &this->showTextureWindow_Specular,
                                  &this->showTexture_Specular,
                                  mmf->meshModel.ModelMaterial.TextureSpecular.Image.c_str());
        }
        if (mmf->meshModel.ModelMaterial.TextureSpecularExp.Image != "") {
            this->showTextureLine("##007",
                                  "SpecularExp",
                                  &mmf->meshModel.ModelMaterial.TextureSpecularExp.UseTexture,
                                  &this->showTextureWindow_SpecularExp,
                                  &this->showTexture_SpecularExp,
                                  mmf->meshModel.ModelMaterial.TextureSpecularExp.Image.c_str());
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
                ImGui::Checkbox("Cel Shading", &(*meshModelFaces)[this->selectedObject]->Setting_CelShading);
                ImGui::Checkbox("Wireframe", &(*meshModelFaces)[this->selectedObject]->Setting_Wireframe);
                // alpha
                ImGui::TextColored(ImVec4(1, 1, 1, (*meshModelFaces)[this->selectedObject]->Setting_Alpha), "Alpha Blending");
                ImGui::SliderFloat("", &(*meshModelFaces)[this->selectedObject]->Setting_Alpha, 0.0f, 1.0f);
                break;
            }
            case 1: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale Model");
                this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[this->selectedObject]->scaleX->animate, &(*meshModelFaces)[this->selectedObject]->scaleX->point, false, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[this->selectedObject]->scaleY->animate, &(*meshModelFaces)[this->selectedObject]->scaleY->point, false, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, (this->managerObjects->Setting_GridSize * 3), true, &(*meshModelFaces)[this->selectedObject]->scaleZ->animate, &(*meshModelFaces)[this->selectedObject]->scaleZ->point, false, isFrame);
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

//                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
                this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects->Setting_GridSize), this->managerObjects->Setting_GridSize, true, &(*meshModelFaces)[this->selectedObject]->positionX->animate, &(*meshModelFaces)[this->selectedObject]->positionX->point, true, isFrame);
//                ImGui::PopItemWidth();
//                ImGui::SameLine();
//                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.35f);
//                ImGui::InputFloat("##0001", &(*meshModelFaces)[this->selectedObject]->positionX->point, -1.0f, -1.0f, 3);
//                ImGui::PopItemWidth();

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
                ImGui::Checkbox("Parallax Mapping", &(*meshModelFaces)[this->selectedObject]->Setting_ParallaxMapping);
                ImGui::Separator();
                ImGui::Checkbox("Use Tessellation", &(*meshModelFaces)[this->selectedObject]->Setting_UseTessellation);
                if ((*meshModelFaces)[this->selectedObject]->Setting_UseTessellation) {
                    ImGui::Checkbox("Culling", &(*meshModelFaces)[this->selectedObject]->Setting_UseCullFace);
                    this->helperUI->addControlsIntegerSlider("Subdivision", 24, 0, 100, &(*meshModelFaces)[this->selectedObject]->Setting_TessellationSubdivision);
                    ImGui::Separator();
                    if (mmf->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
                        this->helperUI->addControlsSlider("Displacement", 15, 0.05f, 0.0f, 10.0f, true, &(*meshModelFaces)[this->selectedObject]->displacementHeightScale->animate, &(*meshModelFaces)[this->selectedObject]->displacementHeightScale->point, false, isFrame);
                        ImGui::Separator();
                    }
                }
                else
                    ImGui::Separator();
                this->helperUI->addControlsSlider("Refraction", 13, 0.05f, -10.0f, 10.0f, true, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialRefraction->animate, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialRefraction->point, true, isFrame);
                this->helperUI->addControlsSlider("Specular Exponent", 14, 10.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialSpecularExp->animate, &(*meshModelFaces)[this->selectedObject]->Setting_MaterialSpecularExp->point, true, isFrame);
                ImGui::Separator();
                this->helperUI->addControlColor3("Ambient Color", &(*meshModelFaces)[this->selectedObject]->materialAmbient->color, &(*meshModelFaces)[this->selectedObject]->materialAmbient->colorPickerOpen);
                this->helperUI->addControlColor3("Diffuse Color", &(*meshModelFaces)[this->selectedObject]->materialDiffuse->color, &(*meshModelFaces)[this->selectedObject]->materialDiffuse->colorPickerOpen);
                this->helperUI->addControlColor3("Specular Color", &(*meshModelFaces)[this->selectedObject]->materialSpecular->color, &(*meshModelFaces)[this->selectedObject]->materialSpecular->colorPickerOpen);
                this->helperUI->addControlColor3("Emission Color", &(*meshModelFaces)[this->selectedObject]->materialEmission->color, &(*meshModelFaces)[this->selectedObject]->materialEmission->colorPickerOpen);
                break;
            }
            case 6: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Effects");

                ImGui::Text("Gaussian Blur");
                const char* gb_items[] = {"No Blur", "Horizontal", "Vertical"};
                ImGui::Combo("Mode##228", &(*meshModelFaces)[this->selectedObject]->Effect_GBlur_Mode, gb_items, IM_ARRAYSIZE(gb_items));
                this->helperUI->addControlsSlider("Radius", 16, 0.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[this->selectedObject]->Effect_GBlur_Radius->animate, &(*meshModelFaces)[this->selectedObject]->Effect_GBlur_Radius->point, true, isFrame);
                this->helperUI->addControlsSlider("Width", 17, 0.0f, 0.0f, 1000.0f, true, &(*meshModelFaces)[this->selectedObject]->Effect_GBlur_Width->animate, &(*meshModelFaces)[this->selectedObject]->Effect_GBlur_Width->point, true, isFrame);
                ImGui::Separator();

//            ImGui::Text("Bloom");
//            ImGui::Checkbox("Apply Bloom", &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_doBloom);
//            this->helperUI->addControlsSlider("Ambient", 18, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_WeightA, false, isFrame);
//            this->helperUI->addControlsSlider("Specular", 19, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_WeightB, false, isFrame);
//            this->helperUI->addControlsSlider("Specular Exp", 20, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_WeightC, false, isFrame);
//            this->helperUI->addControlsSlider("Dissolve", 21, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_WeightD, false, isFrame);
//            this->helperUI->addControlsSlider("Vignette", 22, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_Vignette, false, isFrame);
//            this->helperUI->addControlsSlider("Vignette Attenuation", 23, 0.0f, 0.0f, 10.0f, false, NULL, &(*meshModelFaces)[this->selectedObject]->Effect_Bloom_VignetteAtt, false, isFrame);
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
                ImGui::Combo("##987", &(*meshModelFaces)[this->selectedObject]->materialIlluminationModel, illum_models_items, IM_ARRAYSIZE(illum_models_items));
                break;
            }
            default:
                break;
        }
    }

    ImGui::PopItemWidth();

    ImGui::EndChild();
}

void DialogControlsModels::drawCreate() {
    // https://developer.apple.com/library/mac/samplecode/OpenGLFilterBasicsCocoa/Listings/Sources_Models_Torus_Classes_OpenGLTorusGenerator_cpp.html

    if (ImGui::Button("Triangle", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Triangle);
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

    if (ImGui::Button("Directional (Sun)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Directional);
    if (ImGui::Button("Point (Light bulb)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Point);
    if (ImGui::Button("Spot (Flashlight)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Spot);
}

void DialogControlsModels::contextModelRename(std::vector<ModelFace*> * meshModelFaces) {
    ImGui::OpenPopup("Rename");

    ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Type the new model name:");
    ImGui::Text("(%s)", (*meshModelFaces)[this->selectedObject]->meshModel.ModelTitle.c_str());

    if (this->guiModelRenameText[0] == '\0')
        strcpy(this->guiModelRenameText, (*meshModelFaces)[this->selectedObject]->meshModel.ModelTitle.c_str());
    ImGui::InputText("", this->guiModelRenameText, sizeof(this->guiModelRenameText));

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*meshModelFaces)[this->selectedObject]->meshModel.ModelTitle = std::string(this->guiModelRenameText);
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
    ImGui::Text("(%s)", (*meshModelFaces)[this->selectedObject]->meshModel.ModelTitle.c_str());

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        this->funcDeleteModel(this->selectedObject);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) { ImGui::CloseCurrentPopup(); this->cmenu_deleteYn = false; }

    ImGui::EndPopup();
}

