//
//  UVEditor.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/UVEditor.hpp"
#include "kuplung/ui/iconfonts/IconsFontAwesome.h"
#include "kuplung/utilities/stb/stb_image.h"

void UVEditor::init(int positionX, int positionY, int width, int height) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    this->textureWidth = -1;
    this->textureHeight = -1;
    this->showFileBrowser = false;
    this->textureLoaded = false;
    this->texturePath = "";
    this->textureImage = "";
    this->textureFilename = "";

    this->componentFileBrowser = new FileBrowser();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, 50, 50,
                                     Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height,
                                     std::bind(&UVEditor::dialogFileBrowserProcessFile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    this->componentFileBrowser->setImageBrowser(true);

}

void UVEditor::setModel(ModelFace *mmf, MaterialTextureType texType, std::string texturePath, std::function<void(ModelFace*, MaterialTextureType, std::string, std::vector<glm::vec2>)> processTexture) {
    this->mmf = mmf;
    this->textureType = texType;
    this->texturePath = texturePath;
    this->funcProcessTexture = processTexture;
}

void UVEditor::draw(const char* title, bool* p_opened) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

    ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders);
    ImGui::BeginChild("UVEditor");

    std::string btnLabel = ICON_FA_EYE " Select " + Kuplung_getTextureName(this->textureType) + " Texture Image";
    if (ImGui::Button(btnLabel.c_str()))
        this->showFileBrowser = true;

    ImGui::SameLine();

    if (ImGui::Button("Clear Selected Texture")) {
        this->textureLoaded = false;
        this->textureFilename = "";
        this->textureImage = "";
        this->texturePath = "";
        this->textureHeight = -1;
        this->textureWidth = 1;
    }

    ImGui::SameLine(ImGui::GetWindowWidth() * 0.85);
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3/7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3/7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3/7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Apply Texture")) {
        std::vector<glm::vec2> uvs;
        uvs.push_back(glm::vec2(1.0, 0.0));
        uvs.push_back(glm::vec2(1.0, 1.0));
        uvs.push_back(glm::vec2(0.0, 1.0));
        uvs.push_back(glm::vec2(0.0, 0.0));

        std::vector<glm::vec2> textureCoordinates;
        for (int i=0; i<this->mmf->meshModel.countIndices; i++) {
            if (i % 2 == 0) {
                textureCoordinates.push_back(uvs[3]);
                textureCoordinates.push_back(uvs[0]);
                textureCoordinates.push_back(uvs[2]);
            }
            else {
                textureCoordinates.push_back(uvs[0]);
                textureCoordinates.push_back(uvs[1]);
                textureCoordinates.push_back(uvs[2]);
            }
        }
        this->funcProcessTexture(this->mmf, this->textureType, this->textureImage, textureCoordinates);
    }
    ImGui::PopStyleColor(3);

    ImGui::Separator();

    if (this->textureImage != "") {
        ImGui::Text("Image: %s", this->textureImage.c_str());
        ImGui::Text("Image dimensions: %i x %i", this->textureWidth, this->textureHeight);

        ImGui::Separator();

        ImGui::Image((ImTextureID)(intptr_t)this->vboTexture, ImVec2(this->textureWidth, this->textureHeight));
    }

    ImGui::EndChild();
    ImGui::End();

    if (this->showFileBrowser)
        this->componentFileBrowser->draw("File Browser", &this->showFileBrowser, this->textureType);
}

void UVEditor::dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType) {
    this->showFileBrowser = false;
    this->textureImage = file.path;
    this->textureFilename = file.title;
    strcpy(this->filePath, this->textureImage.c_str());

    if (!boost::filesystem::exists(this->textureImage))
        this->textureImage = Settings::Instance()->currentFolder + "/" + this->textureImage;
    int tChannels;
    unsigned char* tPixels = stbi_load(this->textureImage.c_str(), &this->textureWidth, &this->textureHeight, &tChannels, 0);
    if (!tPixels)
        Settings::Instance()->funcDoLog("[UVEditor] Can't load texture image - " + this->textureImage + " with error - " + std::string(stbi_failure_reason()));
    else {
        glGenTextures(1, &this->vboTexture);
        glBindTexture(GL_TEXTURE_2D, this->vboTexture);
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
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, this->textureWidth, this->textureHeight, 0, textureFormat, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tPixels);

        this->textureLoaded = true;
    }
}
