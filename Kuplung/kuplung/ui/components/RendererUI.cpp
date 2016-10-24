//
//  RendererUI.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "RendererUI.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <boost/filesystem.hpp>
#include "kuplung/utilities/stb/stb_image.h"
#include <boost/algorithm/string/predicate.hpp>
#include "kuplung/utilities/stb/stb_image_write.h"

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x+rhs.x, lhs.y+rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x-rhs.x, lhs.y-rhs.y); }

void RendererUI::init(SDL_Window *sdlWindow) {
    this->wPadding = 20;
    this->bHeight = 40.0f;
    this->panelWidth_RenderOptions = 140.0f;
    this->panelWidth_RenderOptionsMin = 140.0f;
    this->zoomFactor = 1.0f;
    this->rendererType = 2;

    SDL_GetWindowSize(sdlWindow, &this->wWidth, &this->wHeight);
    this->wWidth -= (this->wPadding * 2);
    this->wHeight -= (this->wPadding * 2);
    this->tWidth = 0;
    this->tHeight = 0;

    this->showSaveDialog = false;
    this->componentFileSaver = std::make_unique<FileSaver>();
    this->componentFileSaver->init(100, 100, Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height, std::bind(&RendererUI::dialogFileSaveProcessFile, this, std::placeholders::_1, std::placeholders::_2));
}

void RendererUI::render(bool* show, ImageRenderer *imageRenderer, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> *meshModelFaces) {
    ImGui::SetNextWindowSize(ImVec2(this->wWidth, this->wHeight), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiSetCond_FirstUseEver);

    ImGui::Begin("Renderer", show, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar);

    ImGuiWindow* window = ImGui::GetCurrentWindow();
    const ImGuiIO io = ImGui::GetIO();
    ImGuiContext& g = *GImGui;
    float new_font_scale = ImClamp(this->zoomFactor + g.IO.MouseWheel * 0.10f, 0.50f, 2.50f);

    ImGui::BeginChild("OptionsPanel", ImVec2(this->panelWidth_RenderOptions, 0));
    ImGui::BeginChild("RenderOptions", ImVec2(-1.0f, ImGui::GetWindowHeight() * 0.92));
    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Options");
    ImGui::Separator();
    ImGui::PushItemWidth(-1.0f);
    ImGui::Text("Renderer");
    const char* renderer_items[] = {
        "Scene",
        "Default - Forward",
        "Default - Deferred"
    };
    ImGui::Combo("##987", &this->rendererType, renderer_items, IM_ARRAYSIZE(renderer_items));
    ImGui::Separator();
    ImGui::Text("Image Format");
    const char* image_format_items[] = {
        "BMP",
        "PNG",
        "TGA"
    };
    ImGui::Combo("##988", &this->imageFormat, image_format_items, IM_ARRAYSIZE(image_format_items));

    if (this->currentFileImage != "") {
        ImGui::Separator();
        char ifn[1024];
        strcpy(ifn, this->currentFileImage.c_str());
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Temp File: ");
        ImGui::InputText("##fileName", ifn, IM_ARRAYSIZE(ifn), ImGuiInputTextFlags_ReadOnly);
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Size: ");
        ImGui::Text("%ix%i px", this->tWidth, this->tHeight);
        ImGui::Separator();
    }
    switch (this->rendererType) {
        case 0:
            imageRenderer->showSpecificSettings(ImageRendererType_Scene);
            break;
        case 1:
            imageRenderer->showSpecificSettings(ImageRendererType_DefaultForward);
            break;
        case 2:
            imageRenderer->showSpecificSettings(ImageRendererType_DefaultDeferred);
            break;
        default:
            break;
    }

    ImGui::PopItemWidth();

    ImGui::Separator();
    if (ImGui::Button("Render", ImVec2(-1.0f, this->bHeight))) {
        this->genTexture = true;
        this->renderImageTexture(imageRenderer, managerObjects, meshModelFaces);
    }

    if (ImGui::Button("Save", ImVec2(-1.0f, this->bHeight)))
        this->showSaveDialog = true;
    ImGui::EndChild();
    ImGui::Separator();
    ImGui::BeginChild("RenderHelp", ImVec2(-1.0f, 0.0f));
    ImGui::Text("LMB to drag");
    ImGui::Text("MMB to zoom");
    ImGui::Text("MMB click reset");
    ImGui::EndChild();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::GetIO().MouseDrawCursor = true;
    ImGui::PushStyleColor(ImGuiCol_Button, ImColor(89, 91, 94));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor(119, 122, 124));
    ImGui::PushStyleColor(ImGuiCol_Border, ImColor(0, 0, 0));
    ImGui::Button("###splitterOptions", ImVec2(8.0f, -1));
    ImGui::PopStyleColor(3);
    if (ImGui::IsItemActive()) {
        this->panelWidth_RenderOptions += ImGui::GetIO().MouseDelta.x;
        if (this->panelWidth_RenderOptions < this->panelWidth_RenderOptionsMin)
            this->panelWidth_RenderOptions = this->panelWidth_RenderOptionsMin;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetMouseCursor(4);

    ImGui::SameLine();

    ImGui::BeginChild("RenderWindow", ImVec2(-1.0f, -1.0f), false);
    ImGui::BeginChild("RenderPreview", ImVec2(-1.0f, ImGui::GetWindowHeight() * 0.98), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (ImGui::IsMouseDragging(0)) {
        ImVec2 dragging_location = ImVec2(io.MouseDelta.x / window->FontWindowScale, io.MouseDelta.y / window->FontWindowScale);
        ImGui::SetScrollX(ImGui::GetScrollX() - dragging_location.x);
        ImGui::SetScrollY(ImGui::GetScrollY() - dragging_location.y);
    }

    if (io.MouseClicked[2])
        new_font_scale = 1.f;   // MMB = RESET ZOOM
    float scale = new_font_scale / this->zoomFactor;
    if (scale != 1)	{
        this->scrolling = ImVec2(this->scrolling.x * scale, this->scrolling.y * scale);
        this->zoomFactor = new_font_scale;
    }

    ImGui::Image((ImTextureID)(intptr_t)this->vboBuffer, ImVec2(this->tWidth * this->zoomFactor, this->tHeight * this->zoomFactor));

    // Scrolling
    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
        this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

    ImGui::EndChild();
    ImGui::Text("Zoom: %.2f %%", (this->zoomFactor * 100.0f));
    ImGui::EndChild();

    if (this->showSaveDialog)
        this->dialogFileSave();

    ImGui::End();
    this->genTexture = false;
}

//void RendererUI::render(bool* show, ImageRenderer *imageRenderer, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> *meshModelFaces) {
//    ImGui::SetNextWindowSize(ImVec2(this->wWidth, this->wHeight), ImGuiSetCond_FirstUseEver);
//    ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiSetCond_FirstUseEver);

//    ImGui::Begin("Renderer", show, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar);

//    ImGui::Text("Renderer");
//    ImGui::SameLine();
//    const char* renderer_items[] = {
//        "Default",
//        "Simple",
//        "Complex"
//    };
//    ImGui::Combo("##987", &this->rendererType, renderer_items, IM_ARRAYSIZE(renderer_items));

//    ImGui::Text("Image Format");
//    ImGui::SameLine();
//    const char* image_format_items[] = {
//        "BMP",
//        "PNG",
//        "TGA"
//    };
//    ImGui::Combo("##988", &this->imageFormat, image_format_items, IM_ARRAYSIZE(image_format_items));

//    if (ImGui::Button("Render", ImVec2(ImGui::GetWindowWidth() * 0.20, this->bHeight))) {
//        this->genTexture = true;
//        this->renderImageTexture(imageRenderer, managerObjects, meshModelFaces);
//    }
//    ImGui::SameLine();
//    if (ImGui::Button("Save", ImVec2(ImGui::GetWindowWidth() * 0.78, this->bHeight)))
//        this->showSaveDialog = true;

//    ImGui::Separator();

//    if (this->currentFileImage != "") {
//        char ifn[1024];
//        strcpy(ifn, this->currentFileImage.c_str());
//        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Temp File: "); ImGui::SameLine(); ImGui::InputText("##fileName", ifn, IM_ARRAYSIZE(ifn), ImGuiInputTextFlags_ReadOnly);
//        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Size: "); ImGui::SameLine(); ImGui::Text("%ix%i px", this->tWidth, this->tHeight);
//        ImGui::Separator();
//    }

//    ImGui::Image((ImTextureID)(intptr_t)this->vboBuffer, ImVec2(this->tWidth, this->tHeight), ImVec2(0,0), ImVec2(1,1), ImVec4(1,1,1,1), ImVec4(1,1,1,1));

//    if (this->showSaveDialog)
//        this->dialogFileSave();

//    ImGui::End();
//    this->genTexture = false;
//}

void RendererUI::renderImageTexture(ImageRenderer *imageRenderer, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> *meshModelFaces) {
    FBEntity file;
    file.extension = ".bmp";
    file.isFile = true;

    boost::filesystem::path tempDir = boost::filesystem::temp_directory_path();
    this->currentFileImage = tempDir.string() + "KuplungRendererUI_TempImage.bmp";
    file.path = this->currentFileImage;

    this->currentFileImage = imageRenderer->renderImage(ImageRendererType(this->rendererType), file, meshModelFaces);

    if (this->genTexture)
        this->createTextureBuffer();
}

void RendererUI::createTextureBuffer() {
    if (!boost::filesystem::exists(this->currentFileImage))
        this->currentFileImage = Settings::Instance()->currentFolder + "/" + this->currentFileImage;
    int tChannels;
    unsigned char* tPixels = stbi_load(this->currentFileImage.c_str(), &this->tWidth, &this->tHeight, &tChannels, 0);
    if (!tPixels)
        Settings::Instance()->funcDoLog("[Kuplung-RendererUI] Can't load texture image - " + this->currentFileImage + " with error - " + std::string(stbi_failure_reason()));
    else {
        glGenTextures(1, &this->vboBuffer);
        glBindTexture(GL_TEXTURE_2D, this->vboBuffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->tWidth, this->tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(tPixels);
        this->tWidth += 20;
        this->tHeight += 20;
    }
}

void RendererUI::dialogFileSave() {
    this->componentFileSaver->draw("Save Image", FileSaverOperation_Renderer, &this->showSaveDialog);
}

void RendererUI::dialogFileSaveProcessFile(FBEntity file, FileSaverOperation operation) {
    this->showSaveDialog = false;
    std::string endFile = file.path;

    int width = Settings::Instance()->SDL_Window_Width;
    int height = Settings::Instance()->SDL_Window_Height;

    unsigned char* pixels = new unsigned char[3 * width * height];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glGetTexImage(GL_TEXTURE_2D, this->vboBuffer, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    unsigned char* line_tmp = new unsigned char[3 * width];
    unsigned char* line_a = pixels;
    unsigned char* line_b = pixels + (3 * width * (height - 1));
    while (line_a < line_b) {
        memcpy(line_tmp, line_a, width * 3);
        memcpy(line_a, line_b, width * 3);
        memcpy(line_b, line_tmp, width * 3);
        line_a += width * 3;
        line_b -= width * 3;
    }

    if (this->imageFormat == 0) {
        if (!boost::ends_with(endFile, ".bmp"))
            endFile += ".bmp";
        stbi_write_bmp(endFile.c_str(), width, height, 3, pixels);
    }
    else if (this->imageFormat == 1) {
        if (!boost::ends_with(endFile, ".png"))
            endFile += ".png";
        stbi_write_png(endFile.c_str(), width, height, 3, pixels, width * 3);
    }
    else if (this->imageFormat == 2) {
        if (!boost::ends_with(endFile, ".tga"))
            endFile += ".tga";
        stbi_write_tga(endFile.c_str(), width, height, 3, pixels);
    }

    delete[] pixels;
    delete[] line_tmp;
}
