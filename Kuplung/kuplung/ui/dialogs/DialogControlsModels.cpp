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

DialogControlsModels::DialogControlsModels(ObjectsManager &managerObjects)
    : managerObjects(managerObjects),
      helperUI(std::make_unique<UIHelpers>()),
      componentMaterialEditor(std::make_unique<MaterialEditor>()),
      componentUVEditor(std::make_unique<UVEditor>()) {
    this->managerObjects = managerObjects;

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
    this->showTextureWindow_Displacement = false;
    this->showTexture_Displacement = false;
    this->showUVEditor = false;

    this->textureAmbient_Width = this->textureAmbient_Height = this->textureDiffuse_Width = this->textureDiffuse_Height = 0;
    this->textureDissolve_Width = this->textureDissolve_Height = this->textureBump_Width = this->textureBump_Height = 0;
    this->textureSpecular_Width = this->textureSpecular_Height = this->textureSpecularExp_Width = this->textureSpecularExp_Height = 0;
    this->textureDisplacement_Width = this->textureDisplacement_Height = 0;

    this->selectedObject = -1;
    this->selectedTabScene = -1;
    this->selectedTabGUICamera = -1;
    this->selectedTabGUIGrid = -1;
    this->selectedTabGUILight = -1;
    this->selectedTabPanel = 1;

    this->componentMaterialEditor->init();
    this->componentUVEditor->init(50, 50, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height);
}

void DialogControlsModels::init(SDL_Window* sdlWindow, std::function<void(ShapeType)> addShape, std::function<void(LightSourceType)> addLight, std::function<void(int)> deleteModel) {
    this->sdlWindow = sdlWindow;
    this->funcAddShape = addShape;
    this->funcAddLight = addLight;
    this->funcDeleteModel = deleteModel;
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
        GLenum textureFormat = 0;
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
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(textureFormat), *width, *height, 0, textureFormat, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tPixels);
    }
}

void DialogControlsModels::showTextureLine(std::string const& chkLabel, MaterialTextureType texType, bool* showWindow, bool* loadTexture) {
    ModelFaceBase *mmf = (*this->meshModelFaces)[size_t(this->selectedObject)];
    std::string image, title;
    bool * useTexture = nullptr;
    assert(texType == MaterialTextureType_Ambient ||
           texType == MaterialTextureType_Diffuse ||
           texType == MaterialTextureType_Dissolve ||
           texType == MaterialTextureType_Bump ||
           texType == MaterialTextureType_Specular ||
           texType == MaterialTextureType_SpecularExp ||
           texType == MaterialTextureType_Displacement);
    switch (texType) {
        case MaterialTextureType_Ambient: {
            title = "Ambient";
            useTexture = &mmf->meshModel.ModelMaterial.TextureAmbient.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureAmbient.Image;
            break;
        }
        case MaterialTextureType_Diffuse: {
            title = "Diffuse";
            useTexture = &mmf->meshModel.ModelMaterial.TextureDiffuse.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureDiffuse.Image;
            break;
        }
        case MaterialTextureType_Dissolve: {
            title = "Dissolve";
            useTexture = &mmf->meshModel.ModelMaterial.TextureDissolve.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureDissolve.Image;
            break;
        }
        case MaterialTextureType_Bump: {
            title = "Normal";
            useTexture = &mmf->meshModel.ModelMaterial.TextureBump.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureBump.Image;
            break;
        }
        case MaterialTextureType_Specular: {
            title = "Specular";
            useTexture = &mmf->meshModel.ModelMaterial.TextureSpecular.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureSpecular.Image;
            break;
        }
        case MaterialTextureType_SpecularExp: {
            title = "Specular Exp";
            useTexture = &mmf->meshModel.ModelMaterial.TextureSpecularExp.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureSpecularExp.Image;
            break;
        }
        case MaterialTextureType_Displacement: {
            title = "Displacement";
            useTexture = &mmf->meshModel.ModelMaterial.TextureDisplacement.UseTexture;
            image = mmf->meshModel.ModelMaterial.TextureDisplacement.Image;
            break;
        }
        case MaterialTextureType_Undefined: {
            title = "Undefined";
            image.clear();
            break;
        }
    }

    if (!image.empty()) {
        ImGui::Checkbox(chkLabel.c_str(), useTexture);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", ("Show/Hide " + title + " Texture").c_str());
        ImGui::SameLine();
        if (ImGui::Button((ICON_FA_TIMES + chkLabel).c_str())) {
            *loadTexture = false;
            assert(texType == MaterialTextureType_Ambient ||
                   texType == MaterialTextureType_Dissolve ||
                   texType == MaterialTextureType_Bump ||
                   texType == MaterialTextureType_Specular ||
                   texType == MaterialTextureType_SpecularExp ||
                   texType == MaterialTextureType_Displacement ||
                   texType == MaterialTextureType_Undefined);
            switch (texType) {
                case MaterialTextureType_Ambient: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureAmbient.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureAmbient.Image.clear();
                    break;
                }
                case MaterialTextureType_Diffuse: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDiffuse.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDiffuse.Image.clear();
                    break;
                }
                case MaterialTextureType_Dissolve: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDissolve.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDissolve.Image.clear();
                    break;
                }
                case MaterialTextureType_Bump: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureBump.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureBump.Image.clear();
                    break;
                }
                case MaterialTextureType_Specular: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureSpecular.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureSpecular.Image.clear();
                    break;
                }
                case MaterialTextureType_SpecularExp: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureSpecularExp.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureSpecularExp.Image.clear();
                    break;
                }
                case MaterialTextureType_Displacement: {
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDisplacement.UseTexture = false;
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelMaterial.TextureDisplacement.Image.clear();
                    break;
                }
                case MaterialTextureType_Undefined: {
                    break;
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button((ICON_FA_EYE + chkLabel).c_str())) {
            *showWindow = !*showWindow;
            *loadTexture = true;
        }
        ImGui::SameLine();
        if (ImGui::Button((ICON_FA_PENCIL + chkLabel).c_str())) {
            this->componentUVEditor->setModel((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)], texType, "", std::bind(&DialogControlsModels::processTexture, this, std::placeholders::_1));
            this->showUVEditor = true;
        }
        ImGui::SameLine();
        size_t f = image.find_last_of('/');
        if (f != std::string::npos)
            image = image.substr(f + 1);
        ImGui::Text("%s: %s", title.c_str(), image.c_str());
    }
    else {
        std::string btnLabel = ICON_FA_EYE " Add Texture " + Kuplung_getTextureName(texType);
        if (ImGui::Button(btnLabel.c_str())) {
            this->showUVEditor = true;
            this->componentUVEditor->setModel((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)], texType, "", std::bind(&DialogControlsModels::processTexture, this, std::placeholders::_1));
        }
    }
}

void DialogControlsModels::showTextureImage(ModelFaceBase* mmf, MaterialTextureType type, std::string title, bool* showWindow, bool* genTexture, GLuint* vboBuffer, int* width, int* height) {
    int wWidth, wHeight, tWidth, tHeight, posX, posY;
    SDL_GetWindowSize(this->sdlWindow, &wWidth, &wHeight);

    std::string img("");
    if (type == MaterialTextureType_Ambient)
        img = mmf->meshModel.ModelMaterial.TextureAmbient.Image;
    else if (type == MaterialTextureType_Diffuse)
        img = mmf->meshModel.ModelMaterial.TextureDiffuse.Image;
    else if (type == MaterialTextureType_Dissolve)
        img = mmf->meshModel.ModelMaterial.TextureDissolve.Image;
    else if (type == MaterialTextureType_Bump)
        img = mmf->meshModel.ModelMaterial.TextureBump.Image;
    else if (type == MaterialTextureType_Displacement)
        img = mmf->meshModel.ModelMaterial.TextureDisplacement.Image;
    else if (type == MaterialTextureType_Specular)
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

    ImGui::Image(ImTextureID(intptr_t(*vboBuffer)), ImVec2(*width, *height));

    ImGui::End();
    *genTexture = false;
}

void DialogControlsModels::render(bool* show, bool* isFrame, std::vector<ModelFaceBase*> * mmfaces, int * sceneSelectedModelObject) {
    ImGui::SetNextWindowSize(ImVec2(300, 660), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(10, 28), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Scene Settings", show, ImGuiWindowFlags_ShowBorders);

    ImGui::BeginChild("tabs_list", ImVec2(-1, this->panelHeight_Tabs));
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f));

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
            if (mmfaces != NULL && mmfaces->size() > 0)
                this->drawModels(isFrame, mmfaces);
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

    if (this->selectedObject > -1 && mmfaces != NULL && static_cast<int>((*mmfaces).size()) > this->selectedObject && (*mmfaces)[static_cast<size_t>(this->selectedObject)]->showMaterialEditor)
        this->componentMaterialEditor->draw(this->selectedObject, (*mmfaces)[static_cast<size_t>(this->selectedObject)], &(*mmfaces)[static_cast<size_t>(this->selectedObject)]->showMaterialEditor);

    *sceneSelectedModelObject = this->selectedObject;

    if (this->showUVEditor)
        this->componentUVEditor->draw("UV Editor", &this->showUVEditor);

    ImGui::End();
}

void DialogControlsModels::processTexture(ModelFaceBase *mmf) {
    this->showUVEditor = false;
    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)] = mmf;
}

void DialogControlsModels::drawModels(bool* isFrame, std::vector<ModelFaceBase*> * mmfaces) {
    this->meshModelFaces = mmfaces;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Reset values to default", ImVec2(-1, 0))) {
        for (size_t i=0; i<this->meshModelFaces->size(); i++) {
            (*this->meshModelFaces)[i]->initModelProperties();
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::BeginChild("Scene Items", ImVec2(0, this->heightTopPanel), true);

    std::vector<const char*> scene_items;
    for (size_t i=0; i<this->meshModelFaces->size(); i++) {
        scene_items.push_back((*this->meshModelFaces)[i]->meshModel.ModelTitle.c_str());
    }

    // Scene ModelFace
    ImGui::PushItemWidth(-1);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor(1, 0, 0, 1));
    ImGui::ListBox("", &this->selectedObject, &scene_items[0], static_cast<int>(this->meshModelFaces->size()));
    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(2);
    if (this->selectedObject > -1 && ImGui::BeginPopupContextItem("Actions")) {
        ImGui::MenuItem("Rename", NULL, &this->cmenu_renameModel);
        if (ImGui::MenuItem("Duplicate"))
            (*this->meshModelFaces).push_back((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->clone(static_cast<int>((*this->meshModelFaces).size() + 1)));
        //ImGui::MenuItem("Delete", NULL, &this->cmenu_deleteYn);
        if (ImGui::BeginMenu("Delete")) {
            if (ImGui::MenuItem("OK"))
                this->funcDeleteModel(this->selectedObject);
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    ImGui::PopItemWidth();

    if (this->cmenu_renameModel)
        this->contextModelRename(this->meshModelFaces);

    if (this->cmenu_deleteYn)
        this->contextModelDelete();
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
        ModelFaceBase *mmf = (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)];
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "OBJ File:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.File.title.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "ModelFace:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.ModelTitle.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Material:"); ImGui::SameLine(); ImGui::Text("%s", mmf->meshModel.MaterialTitle.c_str());
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Vertices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countVertices);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Normals:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countNormals);
        ImGui::TextColored(ImVec4(255, 0, 0, 255), "Indices:"); ImGui::SameLine(); ImGui::Text("%i", mmf->meshModel.countIndices);

        if (!mmf->meshModel.ModelMaterial.TextureAmbient.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureDiffuse.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureBump.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureDisplacement.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureDissolve.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureSpecular.Image.empty() ||
            !mmf->meshModel.ModelMaterial.TextureSpecularExp.Image.empty()) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(255, 0, 0, 255), "Textures");
        }

        this->showTextureLine("##001", MaterialTextureType_Ambient, &this->showTextureWindow_Ambient, &this->showTexture_Ambient);
        this->showTextureLine("##002", MaterialTextureType_Diffuse, &this->showTextureWindow_Diffuse, &this->showTexture_Diffuse);
        this->showTextureLine("##003", MaterialTextureType_Dissolve, &this->showTextureWindow_Dissolve, &this->showTexture_Dissolve);
        this->showTextureLine("##004", MaterialTextureType_Bump, &this->showTextureWindow_Bump, &this->showTexture_Bump);
        this->showTextureLine("##005", MaterialTextureType_Displacement, &this->showTextureWindow_Displacement, &this->showTexture_Displacement);
        this->showTextureLine("##006", MaterialTextureType_Specular, &this->showTextureWindow_Specular, &this->showTexture_Specular);
        this->showTextureLine("##007", MaterialTextureType_SpecularExp, &this->showTextureWindow_SpecularExp, &this->showTexture_SpecularExp);

        if (this->showTextureWindow_Ambient)
            this->showTextureImage(mmf, MaterialTextureType_Ambient, "Ambient", &this->showTextureWindow_Ambient, &this->showTexture_Ambient, &this->vboTextureAmbient, &this->textureAmbient_Width, &this->textureAmbient_Height);

        if (this->showTextureWindow_Diffuse)
            this->showTextureImage(mmf, MaterialTextureType_Diffuse, "Diffuse", &this->showTextureWindow_Diffuse, &this->showTexture_Diffuse, &this->vboTextureDiffuse, &this->textureDiffuse_Width, &this->textureDiffuse_Height);

        if (this->showTextureWindow_Dissolve)
            this->showTextureImage(mmf, MaterialTextureType_Dissolve, "Dissolve", &this->showTextureWindow_Dissolve, &this->showTexture_Dissolve, &this->vboTextureDissolve, &this->textureDissolve_Width, &this->textureDissolve_Height);

        if (this->showTextureWindow_Bump)
            this->showTextureImage(mmf, MaterialTextureType_Bump, "Bump", &this->showTextureWindow_Bump, &this->showTexture_Bump, &this->vboTextureBump, &this->textureBump_Width, &this->textureBump_Height);

        if (this->showTextureWindow_Displacement)
            this->showTextureImage(mmf, MaterialTextureType_Displacement, "Height", &this->showTextureWindow_Displacement, &this->showTexture_Displacement, &this->vboTextureDisplacement, &this->textureDisplacement_Width, &this->textureDisplacement_Height);

        if (this->showTextureWindow_Specular)
            this->showTextureImage(mmf, MaterialTextureType_Specular, "Specular", &this->showTextureWindow_Specular, &this->showTexture_Specular, &this->vboTextureSpecular, &this->textureSpecular_Width, &this->textureSpecular_Height);

        if (this->showTextureWindow_SpecularExp)
            this->showTextureImage(mmf, MaterialTextureType_SpecularExp, "SpecularExp", &this->showTextureWindow_SpecularExp, &this->showTexture_SpecularExp, &this->vboTextureSpecularExp, &this->textureSpecularExp_Width, &this->textureSpecularExp_Height);

        ImGui::Separator();

        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(0.1f / 7.0f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.1f / 7.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(0.1f / 7.0f, 0.8f, 0.8f));

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

        static bool K_DCM_ReadOnly = false;

        switch (this->selectedTabScene) {
            case 0: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Properties");
                // cel shading
                ImGui::Checkbox("Cel Shading", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_CelShading);
                ImGui::Checkbox("Wireframe", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Wireframe);
                ImGui::Checkbox("Edit Mode", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_EditMode);
                ImGui::Checkbox("Shadows", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_ShowShadows);
                // alpha
                ImGui::TextColored(ImVec4(1, 1, 1, (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Alpha), "Alpha Blending");
                this->helperUI->addControlsFloatSlider("", 1, 0.0f, 1.0f, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Alpha);
                break;
            }
            case 1: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Scale ModelFace");
                if (ImGui::Checkbox("Gizmo Scale", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Scale)) {
                    if ((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Scale) {
                        (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Rotate = false;
                        (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Translate = false;
                    }
                }
                ImGui::Checkbox("Scale all", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scale0);
                if ((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scale0) {
                    ImGui::Checkbox("", &K_DCM_ReadOnly); ImGui::SameLine(); ImGui::SliderFloat("##001", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleX->point, 0.05f, this->managerObjects.Setting_GridSize / 2); ImGui::SameLine(); ImGui::Text("X");
                    ImGui::Checkbox("", &K_DCM_ReadOnly); ImGui::SameLine(); ImGui::SliderFloat("##001", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleY->point, 0.05f, this->managerObjects.Setting_GridSize / 2); ImGui::SameLine(); ImGui::Text("Y");
                    ImGui::Checkbox("", &K_DCM_ReadOnly); ImGui::SameLine(); ImGui::SliderFloat("##001", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleZ->point, 0.05f, this->managerObjects.Setting_GridSize / 2); ImGui::SameLine(); ImGui::Text("Z");
                }
                else {
                    this->helperUI->addControlsSliderSameLine("X", 1, 0.05f, 0.0f, this->managerObjects.Setting_GridSize / 2, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleX->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleX->point, false, isFrame);
                    this->helperUI->addControlsSliderSameLine("Y", 2, 0.05f, 0.0f, this->managerObjects.Setting_GridSize / 2, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleY->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleY->point, false, isFrame);
                    this->helperUI->addControlsSliderSameLine("Z", 3, 0.05f, 0.0f, this->managerObjects.Setting_GridSize / 2, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleZ->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->scaleZ->point, false, isFrame);
                }
                break;
            }
            case 2: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Rotate model around axis");
                if (ImGui::Checkbox("Gizmo Rotate", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Rotate)) {
                    if ((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Rotate) {
                        (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Scale = false;
                        (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Translate = false;
                    }
                }
                this->helperUI->addControlsSliderSameLine("X", 4, 1.0f, -180.0f, 180.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateX->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateX->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 5, 1.0f, -180.0f, 180.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateY->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 6, 1.0f, -180.0f, 180.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateZ->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->rotateZ->point, true, isFrame);
                break;
            }
            case 3: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Move model by axis");

                if (ImGui::Checkbox("Gizmo Translate", &(*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Translate)) {
                    if ((*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Translate) {
                        (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Scale = false;
                        (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Gizmo_Rotate = false;
                    }
                }

//                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
                this->helperUI->addControlsSliderSameLine("X", 7, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionX->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionX->point, true, isFrame);
//                ImGui::PopItemWidth();
//                ImGui::SameLine();
//                ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.35f);
//                ImGui::InputFloat("##0001", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionX->point, -1.0f, -1.0f, 3);
//                ImGui::PopItemWidth();

                this->helperUI->addControlsSliderSameLine("Y", 8, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionY->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 9, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionZ->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->positionZ->point, true, isFrame);
                break;
            }
            case 4: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Displace model");
                this->helperUI->addControlsSliderSameLine("X", 10, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceX->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceX->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Y", 11, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceY->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceY->point, true, isFrame);
                this->helperUI->addControlsSliderSameLine("Z", 12, 0.5f, (-1 * this->managerObjects.Setting_GridSize), this->managerObjects.Setting_GridSize, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceZ->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displaceZ->point, true, isFrame);
                break;
            }
            case 5: {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Material of the model");
                if (ImGui::Button("Material Editor"))
                    (*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->showMaterialEditor = true;
                ImGui::Checkbox("Parallax Mapping", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_ParallaxMapping);
                ImGui::Separator();
                ImGui::Checkbox("Use Tessellation", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_UseTessellation);
                if ((*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_UseTessellation) {
                    ImGui::Checkbox("Culling", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_UseCullFace);
                    this->helperUI->addControlsIntegerSlider("Subdivision", 24, 0, 100, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_TessellationSubdivision);
                    ImGui::Separator();
                    if (mmf->meshModel.ModelMaterial.TextureDisplacement.UseTexture) {
                        this->helperUI->addControlsSlider("Displacement", 15, 0.05f, -2.0f, 2.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displacementHeightScale->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->displacementHeightScale->point, false, isFrame);
                        ImGui::Separator();
                    }
                }
                else
                    ImGui::Separator();
                this->helperUI->addControlsSlider("Refraction", 13, 0.05f, -10.0f, 10.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_MaterialRefraction->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_MaterialRefraction->point, true, isFrame);
                this->helperUI->addControlsSlider("Specular Exponent", 14, 10.0f, 0.0f, 1000.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_MaterialSpecularExp->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_MaterialSpecularExp->point, true, isFrame);
                ImGui::Separator();
                this->helperUI->addControlColor3("Ambient Color", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialAmbient->color, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialAmbient->colorPickerOpen);
                this->helperUI->addControlColor3("Diffuse Color", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialDiffuse->color, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialDiffuse->colorPickerOpen);
                this->helperUI->addControlColor3("Specular Color", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialSpecular->color, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialSpecular->colorPickerOpen);
                this->helperUI->addControlColor3("Emission Color", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialEmission->color, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialEmission->colorPickerOpen);
                break;
            }
            case 6: {
                if (ImGui::CollapsingHeader("PBR")) {
                    ImGui::Indent();
                    ImGui::BeginGroup();
                    ImGui::Checkbox("Use PBR", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Rendering_PBR);
                    this->helperUI->addControlsSlider("Metallic", 13, 0.0000001f, 0.0f, 1.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Rendering_PBR_Metallic, true, isFrame);
                    this->helperUI->addControlsSlider("Rougness", 14, 0.0000001f, 0.0f, 1.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Rendering_PBR_Roughness, true, isFrame);
                    this->helperUI->addControlsSlider("AO", 15, 0.0000001f, 0.0f, 1.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Setting_Rendering_PBR_AO, true, isFrame);
                    ImGui::EndGroup();
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader("Gaussian Blur")) {
                    ImGui::Indent();
                    ImGui::BeginGroup();
                    const char* gb_items[] = {"No Blur", "Horizontal", "Vertical"};
                    ImGui::Combo("Mode##228", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_GBlur_Mode, gb_items, IM_ARRAYSIZE(gb_items));
                    this->helperUI->addControlsSlider("Radius", 16, 0.0f, 0.0f, 1000.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_GBlur_Radius->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_GBlur_Radius->point, true, isFrame);
                    this->helperUI->addControlsSlider("Width", 17, 0.0f, 0.0f, 1000.0f, true, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_GBlur_Width->animate, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_GBlur_Width->point, true, isFrame);
                    ImGui::EndGroup();
                    ImGui::Unindent();
                }
                if (ImGui::CollapsingHeader("Tone Mapping")) {
                    ImGui::Indent();
                    ImGui::BeginGroup();
                    ImGui::Checkbox("ACES Film Rec2020", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_ToneMapping_ACESFilmRec2020);
                    ImGui::Checkbox("HDR", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_HDR_Tonemapping);
                    ImGui::EndGroup();
                    ImGui::Unindent();
                }

//            ImGui::Text("Bloom");
//            ImGui::Checkbox("Apply Bloom", &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_doBloom);
//            this->helperUI->addControlsSlider("Ambient", 18, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_WeightA, false, isFrame);
//            this->helperUI->addControlsSlider("Specular", 19, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_WeightB, false, isFrame);
//            this->helperUI->addControlsSlider("Specular Exp", 20, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_WeightC, false, isFrame);
//            this->helperUI->addControlsSlider("Dissolve", 21, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_WeightD, false, isFrame);
//            this->helperUI->addControlsSlider("Vignette", 22, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_Vignette, false, isFrame);
//            this->helperUI->addControlsSlider("Vignette Attenuation", 23, 0.0f, 0.0f, 10.0f, false, NULL, &(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->Effect_Bloom_VignetteAtt, false, isFrame);
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
                ImGui::Combo("##987", reinterpret_cast<int*>(&(*this->meshModelFaces)[static_cast<size_t>(this->selectedObject)]->materialIlluminationModel), illum_models_items, IM_ARRAYSIZE(illum_models_items));
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
    ImGui::Separator();

    if (ImGui::Button("Epcot", ImVec2(-1, 0))) this->funcAddShape(ShapeType_Epcot);
    if (ImGui::Button("Brick Wall", ImVec2(-1, 0))) this->funcAddShape(ShapeType_BrickWall);
    if (ImGui::Button("Plane Objects", ImVec2(-1, 0))) this->funcAddShape(ShapeType_PlaneObjects);
    if (ImGui::Button("Plane Objects - Large Plane", ImVec2(-1, 0))) this->funcAddShape(ShapeType_PlaneObjectsLargePlane);
    if (ImGui::Button("Material Ball", ImVec2(-1, 0))) this->funcAddShape(ShapeType_MaterialBall);
    if (ImGui::Button("Material Ball - Blender", ImVec2(-1, 0))) this->funcAddShape(ShapeType_MaterialBallBlender);

    ImGui::Separator();
    ImGui::Separator();

    if (ImGui::Button("Directional (Sun)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Directional);
    if (ImGui::Button("Point (Light bulb)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Point);
    if (ImGui::Button("Spot (Flashlight)", ImVec2(-1, 0))) this->funcAddLight(LightSourceType_Spot);
}

void DialogControlsModels::contextModelRename(std::vector<ModelFaceBase*> * meshModelFaces) {
    ImGui::OpenPopup("Rename");

    ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Type the new model name:");
    ImGui::Text("(%s)", (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelTitle.c_str());

    if (this->guiModelRenameText[0] == '\0')
        strcpy(this->guiModelRenameText, (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelTitle.c_str());
    ImGui::InputText("", this->guiModelRenameText, sizeof(this->guiModelRenameText));

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelTitle = std::string(this->guiModelRenameText);
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

void DialogControlsModels::contextModelDelete() {
//    ImGui::SetNextWindowPos(ImGui::GetIO().MouseClickedPos[0]);

    ImGui::OpenPopup("Delete?");

    ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Are you sure?\n");
    //ImGui::Text("(%s)", (*meshModelFaces)[static_cast<size_t>(this->selectedObject)]->meshModel.ModelTitle.c_str());

    if (ImGui::Button("OK", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.5f,0))) {
        this->funcDeleteModel(this->selectedObject);
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }
    ImGui::SameLine();
    if (ImGui::Button("No", ImVec2(ImGui::GetContentRegionAvailWidth(),0))) {
        ImGui::CloseCurrentPopup();
        this->cmenu_deleteYn = false;
    }

    ImGui::EndPopup();
}

