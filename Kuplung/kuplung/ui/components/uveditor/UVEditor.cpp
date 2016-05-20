//
//  UVEditor.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/uveditor/UVEditor.hpp"
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
    this->uvUnwrappingType = UVUnwrappingMethod_None;
    this->uvUnwrappingTypePrev = -1;
    this->texturePath = "";
    this->textureImage = "";
    this->textureFilename = "";

    this->componentFileBrowser = new FileBrowser();
    this->componentFileBrowser->init(Settings::Instance()->logFileBrowser, 50, 50,
                                     Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height,
                                     std::bind(&UVEditor::dialogFileBrowserProcessFile, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    this->componentFileBrowser->setImageBrowser(true);

}

void UVEditor::setModel(ModelFace *mmf, MaterialTextureType texType, std::string texturePath, std::function<void(ModelFace*)> processTexture) {
    this->mmf = mmf;
    this->textureType = texType;
    this->texturePath = texturePath;
    this->funcProcessTexture = processTexture;

    FBEntity file;
    file.isFile = true;
    file.modifiedDate = "";
    file.size = "";
    file.extension = "";
    switch (texType) {
        case MaterialTextureType_Ambient: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureAmbient.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureAmbient.Image;
            break;
        }
        case MaterialTextureType_Diffuse: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureDiffuse.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureDiffuse.Image;
            break;
        }
        case MaterialTextureType_Dissolve: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureDissolve.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureDissolve.Image;
            break;
        }
        case MaterialTextureType_Bump: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureBump.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureBump.Image;
            break;
        }
        case MaterialTextureType_Specular: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureSpecular.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureSpecular.Image;
            break;
        }
        case MaterialTextureType_SpecularExp: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Image;
            break;
        }
        case MaterialTextureType_Displacement: {
            file.title = this->mmf->meshModel.ModelMaterial.TextureDisplacement.Image;
            file.path = this->mmf->meshModel.ModelMaterial.TextureDisplacement.Image;
            break;
        }
        default:
            break;
    }
    this->dialogFileBrowserProcessFile(file, (FileBrowser_ParserType)Settings::Instance()->ModelFileParser, this->textureType);
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
    ImGui::Text("Image: %s", (this->textureImage == "" ? "" : this->textureImage.c_str()));
    if (this->textureWidth > 0 && this->textureHeight > 0)
        ImGui::Text("Image dimensions: %i x %i", this->textureWidth, this->textureHeight);
    else
        ImGui::Text("Image dimensions: ");

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImColor(60, 60, 70, 200));
    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;

    // grid
    ImU32 GRID_COLOR = ImColor(200, 200, 200, 40);
    float GRID_SZ = 64.0f * window->FontWindowScale;
    ImVec2 win_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetWindowSize();
    for (float x = fmodf(offset.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
        draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
    for (float y = fmodf(offset.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
        draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);

    if (this->textureImage != "") {
        draw_list->ChannelsSplit(2);

        if (this->uvUnwrappingType != this->uvUnwrappingTypePrev) {
            switch (this->uvUnwrappingType) {
                case UVUnwrappingMethod_Default:
                    this->projectSquare();
                    break;
                default:
                    this->uvPoints.clear();
                    this->uvLines.clear();
                    break;
            }
            this->uvUnwrappingTypePrev = this->uvUnwrappingType;
        }

        // texture image
        draw_list->ChannelsSetCurrent(0);
        ImVec2 bb_min = offset;
        ImVec2 bb_max = ImVec2(this->textureWidth, this->textureHeight) + offset;
        draw_list->AddImage((ImTextureID)(intptr_t)this->vboTexture, bb_min, bb_max);

        draw_list->ChannelsSetCurrent(1);

        if (this->uvPoints.size() > 0) {
            const ImGuiIO io = ImGui::GetIO();
            const ImVec2 mouseScreenPos = io.MousePos;

           // points
            for (size_t i=0; i<this->uvPoints.size(); i++) {
                UVPoint *p = this->uvPoints[i];
                ImGui::PushID(p->ID);

                ImVec2 pointRect = p->position + offset;
                p->draw(pointRect);

                ImGui::SetCursorScreenPos(pointRect);
                if (std::abs(mouseScreenPos.x - pointRect.x) < this->pRadius && std::abs(mouseScreenPos.y - pointRect.y) < this->pRadius)
                    p->isDragging = true;

                if (p->isDragging == true && ImGui::IsMouseDragging(0)) {
                    ImVec2 np_scale = ImVec2(io.MouseDelta.x / window->FontWindowScale, io.MouseDelta.y / window->FontWindowScale);
                    p->position = p->position + np_scale;
                }

                ImGui::PopID();
            }

            // lines
            for (size_t i=0; i<this->uvLines.size(); i++) {
                UVLine l = this->uvLines[i];
                l.positionX = l.positionX + offset;
                l.positionY = l.positionY + offset;
                draw_list->AddLine(l.positionX, l.positionY, l.color);
            }

            // add overlay
            draw_list->AddRectFilled(ImVec2(0.0, 0.0) + offset, ImVec2(this->textureWidth, this->textureHeight) + offset, this->overlayColor);
        }
    }

    draw_list->ChannelsMerge();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    ImGui::EndChild();
    ImGui::End();

    if (this->showFileBrowser)
        this->componentFileBrowser->draw("File Browser", &this->showFileBrowser, this->textureType);
}

void UVEditor::projectSquare() {
    this->uvPoints.clear();
    this->uvLines.clear();

    float ox = 0.0;//10.0;
    float oy = 0.0;//14.0;

    // vertices
    this->uvPoints.push_back(new UVPoint(1, ImVec2(ox, oy), this->pColor, this->pRadius));
    this->uvPoints.push_back(new UVPoint(2, ImVec2(this->textureWidth, oy), this->pColor, this->pRadius));
    this->uvPoints.push_back(new UVPoint(2, ImVec2(ox, this->textureHeight), this->pColor, this->pRadius));
    this->uvPoints.push_back(new UVPoint(2, ImVec2(this->textureWidth, this->textureHeight), this->pColor, this->pRadius));

    // lines
    UVLine l_top_left_to_right;
    l_top_left_to_right.positionX = ImVec2(ox, oy);
    l_top_left_to_right.positionY = ImVec2(this->textureWidth, oy);
    l_top_left_to_right.color = this->lColor;
    this->uvLines.push_back(l_top_left_to_right);

    UVLine l_top_left_to_bottom_right;
    l_top_left_to_bottom_right.positionX = ImVec2(ox, oy);
    l_top_left_to_bottom_right.positionY = ImVec2(oy, this->textureHeight);
    l_top_left_to_bottom_right.color = this->lColor;
    this->uvLines.push_back(l_top_left_to_bottom_right);

    UVLine l_bottom_left_to_bottom_right;
    l_bottom_left_to_bottom_right.positionX = ImVec2(ox, this->textureHeight);
    l_bottom_left_to_bottom_right.positionY = ImVec2(this->textureWidth, this->textureHeight);
    l_bottom_left_to_bottom_right.color = this->lColor;
    this->uvLines.push_back(l_bottom_left_to_bottom_right);

    UVLine l_top_right_to_bottom_right;
    l_top_right_to_bottom_right.positionX = ImVec2(this->textureWidth, oy);
    l_top_right_to_bottom_right.positionY = ImVec2(this->textureWidth, this->textureHeight);
    l_top_right_to_bottom_right.color = this->lColor;
    this->uvLines.push_back(l_top_right_to_bottom_right);
}

void UVEditor::processTextureCoordinatesSquare() {
    if (this->uvPoints.size() > 0) {
        std::vector<glm::vec2> uvs;
        for (size_t i=0; i<this->uvPoints.size(); i++) {
            UVPoint *p = this->uvPoints[i];
            float x = p->position.x / (float)this->textureWidth;
            float y = p->position.y / (float)this->textureHeight;
            uvs.push_back(glm::vec2(x, y));
        }

//        std::vector<glm::vec3> uniqueNormals;
//        for (size_t i=0; i<this->mmf->meshModel.normals.size(); i++) {
//            if (std::find(uniqueNormals.begin(), uniqueNormals.end(), this->mmf->meshModel.normals[i]) == uniqueNormals.end())
//                uniqueNormals.push_back(this->mmf->meshModel.normals[i]);
//        }

//        std::vector<std::vector<glm::vec3>> triangles;
//        std::vector<glm::vec3> triangle;
//        int tCounter = 1;
//        for (size_t i=0; i<this->mmf->meshModel.indices.size(); i++) {
//            glm::vec3 p = this->mmf->meshModel.vertices[i];
//            glm::vec3 n = this->mmf->meshModel.normals[this->mmf->meshModel.indices[i]];
//            triangle.push_back(p);
//            if (tCounter % 3 == 0) {
//                triangle.push_back(n);
//                triangles.push_back(triangle);
//                triangle.clear();
//                tCounter = 1;
//            }
//            else
//                tCounter += 1;
//        }

//        printf("-------------------------------------------------------\n");

//        std::vector<glm::vec2> textureCoordinates;
//        for (size_t i=0; i<triangles.size(); i++) {
//            std::vector<glm::vec3> t = triangles[i];
//            glm::vec3 p1 = t[0];
//            glm::vec3 p2 = t[1];
//            glm::vec3 p3 = t[2];
//            glm::vec3 n = t[3];

//            printf("[%i] = [%.2f, %.2f, %.2f] ---- [%.2f, %.2f, %.2f] / [%.2f, %.2f, %.2f] / [%.2f, %.2f, %.2f]\n",
//                   (int)i,
//                   n.x, n.y, n.z,
//                   p1.x, p1.y, p1.z,
//                   p2.x, p2.y, p2.z,
//                   p3.x, p3.y, p3.z);
//        }
//        this->mmf->meshModel.texture_coordinates = textureCoordinates;
//        this->mmf->meshModel.countTextureCoordinates = (int)textureCoordinates.size();

        std::vector<glm::vec2> textureCoordinates;
        int maxIndice = ((*std::max_element(this->mmf->meshModel.indices.begin(), this->mmf->meshModel.indices.end())) + 1);
        for (int i=0; i<maxIndice / 3; i++) {
            if (i < (maxIndice / 2)) {
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
        this->mmf->meshModel.texture_coordinates = textureCoordinates;
        this->mmf->meshModel.countTextureCoordinates = (int)textureCoordinates.size();
    }

    switch (this->textureType) {
        case MaterialTextureType_Ambient:
            this->mmf->meshModel.ModelMaterial.TextureAmbient.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureAmbient.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureAmbient.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureAmbient.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureAmbient.Height = this->textureHeight;
            break;
        case MaterialTextureType_Bump:
            this->mmf->meshModel.ModelMaterial.TextureBump.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureBump.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureBump.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureBump.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureBump.Height = this->textureHeight;
            break;
        case MaterialTextureType_Diffuse:
            this->mmf->meshModel.ModelMaterial.TextureDiffuse.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureDiffuse.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDiffuse.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDiffuse.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureDiffuse.Height = this->textureHeight;
            break;
        case MaterialTextureType_Displacement:
            this->mmf->meshModel.ModelMaterial.TextureDisplacement.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureDisplacement.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDisplacement.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDisplacement.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureDisplacement.Height = this->textureHeight;
            break;
        case MaterialTextureType_Dissolve:
            this->mmf->meshModel.ModelMaterial.TextureDissolve.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureDissolve.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDissolve.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureDissolve.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureDissolve.Height = this->textureHeight;
            break;
        case MaterialTextureType_Specular:
            this->mmf->meshModel.ModelMaterial.TextureSpecular.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureSpecular.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureSpecular.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureSpecular.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureSpecular.Height = this->textureHeight;
            break;
        case MaterialTextureType_SpecularExp:
            this->mmf->meshModel.ModelMaterial.TextureSpecularExp.UseTexture = true;
            this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Image = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Filename = this->textureImage;
            this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Width = this->textureWidth;
            this->mmf->meshModel.ModelMaterial.TextureSpecularExp.Height = this->textureHeight;
            break;
        default:
            break;
    }

    this->mmf->initBuffersAgain = true;

    std::vector<MeshModel> mm;
    mm.push_back(this->mmf->meshModel);
    Kuplung_printObjModels(mm, false);
}

void UVEditor::processTextureCoordinates() {
    switch (this->uvUnwrappingType) {
        case UVUnwrappingMethod_Default:
            this->processTextureCoordinatesSquare();
            break;
        default:
            break;
    }
    if (this->uvUnwrappingType > 0) {

    }

    this->funcProcessTexture(this->mmf);
}

void UVEditor::initTextureBuffer() {
    this->textureWidth = 0;
    this->textureHeight = 0;

    if (this->textureImage != "") {
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
            this->uvUnwrappingTypePrev = UVUnwrappingMethod_None;
        }
    }
}

void UVEditor::dialogFileBrowserProcessFile(FBEntity file, FileBrowser_ParserType parserType, MaterialTextureType texType) {
    this->showFileBrowser = false;
    this->textureImage = file.path;
    this->textureFilename = file.title;
    strcpy(this->filePath, this->textureImage.c_str());
    this->initTextureBuffer();
}
