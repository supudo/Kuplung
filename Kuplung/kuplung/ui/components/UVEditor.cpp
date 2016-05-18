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
#include "kuplung/utilities/imgui/imgui_internal.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

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
    this->uvUnwrappingType = 0;

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

    std::string btnLabel = ICON_FA_EYE " Browse ...";
    if (ImGui::Button(btnLabel.c_str()))
        this->showFileBrowser = true;

    ImGui::SameLine();

    btnLabel = ICON_FA_TIMES " Clear";
    if (ImGui::Button(btnLabel.c_str())) {
        this->textureLoaded = false;
        this->textureFilename = "";
        this->textureImage = "";
        this->texturePath = "";
        this->textureHeight = -1;
        this->textureWidth = 1;
    }

    ImGui::SameLine(ImGui::GetWindowWidth() * 0.88);
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(3 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(3 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImColor::HSV(3 / 7.0f, 0.8f, 0.8f));
    btnLabel = ICON_FA_CHECK " Apply ";
    if (ImGui::Button(btnLabel.c_str()))
        this->processTextureCoordinates();
    ImGui::PopStyleColor(3);

    const char* mapping_items[] = {
        "-- Select Unwrapping Method --",
        "Square",
        "Cube",
        "Cylinder",
        "Sphere"
    };
    ImGui::Combo("##002", &this->uvUnwrappingType, mapping_items, IM_ARRAYSIZE(mapping_items));

    ImGui::Separator();

    if (this->textureImage != "") {
        ImGui::Text("Image: %s", this->textureImage.c_str());
        ImGui::Text("Image dimensions: %i x %i", this->textureWidth, this->textureHeight);

        ImGui::Separator();

        ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;
        ImVec2 connectorScreenPos = ImVec2(0, 0);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->ChannelsSplit(2);

        // texture image
        draw_list->ChannelsSetCurrent(0);
        connectorScreenPos = ImVec2(10.0, 10.0);
        ImVec2 bb_min = connectorScreenPos + offset;
        ImVec2 bb_max = ImVec2(this->textureWidth, this->textureHeight) + offset;
        draw_list->AddImage((ImTextureID)(intptr_t)this->vboTexture, bb_min, bb_max);

        draw_list->ChannelsSetCurrent(1);

        // points
        for (size_t i=0; i<this->uvPoints.size(); i++) {
            UVPoint p = this->uvPoints[i];
            p.position = p.position + offset;
            draw_list->AddCircleFilled(p.position, p.radius, p.color);
        }

        // lines
        for (size_t i=0; i<this->uvLines.size(); i++) {
            UVLine l = this->uvLines[i];
            l.positionX = l.positionX + offset;
            l.positionY = l.positionY + offset;
            draw_list->AddLine(l.positionX, l.positionY, l.color);
        }

        // add overlay
        draw_list->AddRectFilled(ImVec2(10.0, 14.0) + offset, ImVec2(this->textureWidth, this->textureHeight) + offset, ImColor(255, 112, 0, 100));

        draw_list->ChannelsMerge();
    }

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::EndChild();
    ImGui::End();

    if (this->showFileBrowser)
        this->componentFileBrowser->draw("File Browser", &this->showFileBrowser, this->textureType);
}

void UVEditor::projectSquare() {
    this->uvPoints.clear();
    this->uvLines.clear();

    ImColor pColor = ImColor(255, 112, 0);
    float pRadius = 5.0f;

    // vertices
    UVPoint p_top_left;
    p_top_left.color = pColor;
    p_top_left.radius = pRadius;
    p_top_left.position = ImVec2(10.0, 14.0);
    this->uvPoints.push_back(p_top_left);

    UVPoint p_top_right;
    p_top_right.color = pColor;
    p_top_right.radius = pRadius;
    p_top_right.position = ImVec2(this->textureWidth, 14.0);
    this->uvPoints.push_back(p_top_right);

    UVPoint p_bottom_left;
    p_bottom_left.color = pColor;
    p_bottom_left.radius = pRadius;
    p_bottom_left.position = ImVec2(10.0, this->textureHeight);
    this->uvPoints.push_back(p_bottom_left);

    UVPoint p_bottom_right;
    p_bottom_right.color = pColor;
    p_bottom_right.radius = pRadius;
    p_bottom_right.position = ImVec2(this->textureWidth, this->textureHeight);
    this->uvPoints.push_back(p_bottom_right);

    // lines
    UVLine l_top_left_to_right;
    l_top_left_to_right.positionX = ImVec2(10.0, 14.0);
    l_top_left_to_right.positionY = ImVec2(this->textureWidth, 14.0);
    l_top_left_to_right.color = pColor;
    this->uvLines.push_back(l_top_left_to_right);

    UVLine l_top_left_to_bottom_right;
    l_top_left_to_bottom_right.positionX = ImVec2(10.0, 14.0);
    l_top_left_to_bottom_right.positionY = ImVec2(10.0, this->textureHeight);
    l_top_left_to_bottom_right.color = pColor;
    this->uvLines.push_back(l_top_left_to_bottom_right);

    UVLine l_bottom_left_to_bottom_right;
    l_bottom_left_to_bottom_right.positionX = ImVec2(10.0, this->textureHeight);
    l_bottom_left_to_bottom_right.positionY = ImVec2(this->textureWidth, this->textureHeight);
    l_bottom_left_to_bottom_right.color = pColor;
    this->uvLines.push_back(l_bottom_left_to_bottom_right);

    UVLine l_top_right_to_bottom_right;
    l_top_right_to_bottom_right.positionX = ImVec2(this->textureWidth, 14.0);
    l_top_right_to_bottom_right.positionY = ImVec2(this->textureWidth, this->textureHeight);
    l_top_right_to_bottom_right.color = pColor;
    this->uvLines.push_back(l_top_right_to_bottom_right);
}

void UVEditor::processTextureCoordinates() {
    std::vector<glm::vec2> uvs;
    uvs.push_back(glm::vec2(1.0, 0.0));
    uvs.push_back(glm::vec2(1.0, 1.0));
    uvs.push_back(glm::vec2(0.0, 1.0));
    uvs.push_back(glm::vec2(0.0, 0.0));

    std::vector<glm::vec2> textureCoordinates;
    for (int i=0; i<this->mmf->meshModel.countIndices / 3; i++) {
        if (i < 6) {
            textureCoordinates.push_back(uvs[0]);
            textureCoordinates.push_back(uvs[1]);
            textureCoordinates.push_back(uvs[2]);
        }
        else {
            textureCoordinates.push_back(uvs[3]);
            textureCoordinates.push_back(uvs[0]);
            textureCoordinates.push_back(uvs[2]);
        }
    }

    std::map<PackedVertex2, unsigned int> vertexToOutIndex;
    std::vector<glm::vec3> outVertices, outNormals;
    std::vector<glm::vec2> outTextureCoordinates;
    for (size_t j=0; j<this->mmf->meshModel.vertices.size(); j++) {
        PackedVertex2 packed = { this->mmf->meshModel.vertices[j], textureCoordinates[j], this->mmf->meshModel.normals[j] };

        unsigned int index;
        bool found = this->getSimilarVertexIndex2(packed, vertexToOutIndex, index);
        if (found)
            this->mmf->meshModel.indices.push_back(index);
        else {
            outVertices.push_back(this->mmf->meshModel.vertices[j]);
            outTextureCoordinates.push_back(textureCoordinates[j]);
            outNormals.push_back(this->mmf->meshModel.normals[j]);
            unsigned int newIndex = (unsigned int)outVertices.size() - 1;
            this->mmf->meshModel.indices.push_back(newIndex);
            vertexToOutIndex[packed] = newIndex;
        }
    }
    this->mmf->meshModel.vertices = outVertices;
    this->mmf->meshModel.texture_coordinates = outTextureCoordinates;
    this->mmf->meshModel.countTextureCoordinates = (int)outTextureCoordinates.size();
    this->mmf->meshModel.normals = outNormals;
    this->mmf->meshModel.indices = this->mmf->meshModel.indices;
    this->mmf->meshModel.countIndices = (int)this->mmf->meshModel.indices.size();

    this->mmf->meshModel.ModelMaterial.TextureDiffuse.UseTexture = true;
    this->mmf->meshModel.ModelMaterial.TextureDiffuse.Image = this->textureImage;
    this->mmf->meshModel.ModelMaterial.TextureDiffuse.Filename = this->textureImage;

    std::vector<MeshModel> mm;
    mm.push_back(this->mmf->meshModel);
    Kuplung_printObjModels(mm, false);

    this->funcProcessTexture(this->mmf, this->textureType, this->textureImage, textureCoordinates);
}

bool UVEditor::getSimilarVertexIndex2(PackedVertex2 & packed, std::map<PackedVertex2, unsigned int> & vertexToOutIndex, unsigned int & result) {
    std::map<PackedVertex2, unsigned int>::iterator it = vertexToOutIndex.find(packed);
    if (it == vertexToOutIndex.end())
        return false;
    else {
        result = it->second;
        return true;
    }
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

        this->projectSquare();
    }
}
