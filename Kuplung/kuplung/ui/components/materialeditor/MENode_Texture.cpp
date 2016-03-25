//
//  MENode_Texture.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MENode_Texture.hpp"
#include <math.h>
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/ui/iconfonts/IconsMaterialDesign.h"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <boost/filesystem.hpp>
#include "kuplung/utilities/stb/stb_image.h"

// NB: You can use math functions/operators on ImVec2 if you #define IMGUI_DEFINE_MATH_OPERATORS and #include "imgui_internal.h"
// Here we only declare simple +/- operators so others don't leak into the demo code.
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

MENode_Texture::MENode_Texture(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename, std::string textureImage) {
    this->initBase(id, name, pos, value, color, inputs_count, outputs_count, textureFilename, textureImage);
    this->showTextureWindow = false;
    this->showFileBrowser = false;
    this->loadTexture = false;
    this->textureWidth = 0;
    this->textureHeight = 0;
    strcpy(this->filePath, this->TextureImage.c_str());

    this->componentFileBrowser = new FileBrowser();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, 50, 50,
                                     Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height,
                                     std::bind(&MENode_Texture::doLog, this, std::placeholders::_1),
                                     std::bind(&MENode_Texture::dialogFileBrowserProcessFile, this, std::placeholders::_1));
    this->componentFileBrowser->setImageBrowser(true);

    this->createTextureBuffer(&this->textureWidth, &this->textureHeight);
}

void MENode_Texture::initBase(int id, std::string name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count, std::string textureFilename, std::string textureImage) {
    MENode::init(id, MaterialEditor_NodeType_Image, name, pos, value, color, inputs_count, outputs_count, textureFilename, textureImage);
}

void MENode_Texture::draw(ImVec2 node_rect_min, ImVec2 NODE_WINDOW_PADDING, bool showPreview) {
    ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
    ImGui::BeginGroup();

    ImGui::SetNextTreeNodeOpened(this->IsExpanded, ImGuiSetCond_Always);
    if (ImGui::TreeNode(this, "%s", "")) {
        ImGui::TreePop();
        this->IsExpanded = true;
    }
    else
        this->IsExpanded = false;
    ImGui::SameLine(0,0);
    ImGui::TextColored(ImColor(255, 0, 0), "%s", this->Name.c_str());

    if (this->IsExpanded) {
        ImGui::SliderFloat("##value", &this->Value, 0.0f, 1.0f, "Alpha %.2f");

        std::string btnLabel = ICON_FA_EYE;
        if (ImGui::Button(btnLabel.c_str())) {
            this->showTextureWindow = !this->showTextureWindow;
            this->loadTexture = true;
        }
        ImGui::SameLine();
        ImGui::InputText("", this->filePath, sizeof(this->filePath), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine();
        float bw = ImGui::CalcTextSize("...").x + 10;
        ImGui::PushItemWidth(bw);
        if (ImGui::Button("..."))
            this->showFileBrowser = true;

        if (showPreview && ImGui::ImageButton((ImTextureID)(intptr_t)this->vboBuffer, ImVec2(100, 100)))
            this->showTextureWindow = !this->showTextureWindow;

        ImGui::PopItemWidth();
    }

    ImGui::EndGroup();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    if (this->showTextureWindow)
        this->showImage();

    if (this->showFileBrowser)
        this->componentFileBrowser->draw("File Browser", &this->showFileBrowser);
    ImGui::PopStyleVar(2);
}

void MENode_Texture::showImage() {
    int wWidth, wHeight, tWidth, tHeight, posX, posY;
    //SDL_GetWindowSize(this->sdlWindow, &wWidth, &wHeight);

    wWidth = 800;
    wHeight = 800;

    if (this->loadTexture)
        this->createTextureBuffer(&this->textureWidth, &this->textureHeight);

    tWidth = this->textureWidth + 20;
    if (tWidth > wWidth)
        tWidth = wWidth - 20;

    tHeight = this->textureHeight + 20;
    if (tHeight > wHeight)
        tHeight = wHeight - 40;

    posX = ImGui::GetMousePos().x + 20;
    posY = 20;

    ImGui::SetNextWindowSize(ImVec2(tWidth, tHeight), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Image", &this->showTextureWindow, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::Text("%s", this->TextureFilename.c_str());
    ImGui::Text("Image dimensions: %i x %i", this->textureWidth, this->textureHeight);

    ImGui::Separator();

    ImGui::Image((ImTextureID)(intptr_t)this->vboBuffer, ImVec2(this->textureWidth, this->textureHeight));

    ImGui::End();

    this->loadTexture = false;
}

void MENode_Texture::createTextureBuffer(int* width, int* height) {
    if (!boost::filesystem::exists(this->TextureImage))
        this->TextureImage = Settings::Instance()->currentFolder + "/" + this->TextureImage;
    int tChannels;
    unsigned char* tPixels = stbi_load(this->TextureImage.c_str(), width, height, &tChannels, 0);
    if (!tPixels)
        this->doLog("Can't load texture image - " + this->TextureImage + " with error - " + std::string(stbi_failure_reason()));
    else {
        glGenTextures(1, &this->vboBuffer);
        glBindTexture(GL_TEXTURE_2D, this->vboBuffer);
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

void MENode_Texture::dialogFileBrowserProcessFile(FBEntity file) {
    this->showFileBrowser = false;
    this->TextureImage = file.path;
    this->TextureFilename = file.title;
    strcpy(this->filePath, this->TextureImage.c_str());
    this->initBase(this->ID, this->Name, this->Pos, this->Value, this->Color, this->InputsCount, this->OutputsCount, this->TextureFilename, this->TextureImage);
}

void MENode_Texture::doLog(std::string logMessage) {
    Settings::Instance()->funcDoLog("[MENode_Texture] " + logMessage);
}
