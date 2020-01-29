//
//  DialogShadertoy.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "DialogShadertoy.hpp"
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/stb/stb_image.h"
#include <string>
#include <iostream>
#include <sstream>
#include <stdio.h>

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

void DialogShadertoy::render(bool* p_opened) {
  ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);

/// MIGRATE : ImGui::Begin("Shadertoy.com", p_opened, ImGuiWindowFlags_ShowBorders);
ImGui::Begin("Shadertoy.com", p_opened);

  this->windowWidth = ImGui::GetWindowWidth();
  this->windowHeight = ImGui::GetWindowHeight();
  this->textureWidth = int(this->windowWidth - this->viewPaddingHorizontal);
  this->textureHeight = int(this->windowHeight - this->viewPaddingVertical);

  if (this->heightTopPanel < this->engineShadertoy->textureHeight || this->heightTopPanel > this->engineShadertoy->textureHeight)
    this->engineShadertoy->initFBO(
      int(this->windowWidth),
      int(this->heightTopPanel),
      &this->vboTexture);

  this->engineShadertoy->renderToTexture(
    int(ImGui::GetIO().MousePos.x),
    int(ImGui::GetIO().MousePos.y),
    (SDL_GetTicks() / 1000.0f),
    &this->vboTexture
    );

  // BEGIN preview
  ImGui::BeginChild("Preview", ImVec2(0, this->heightTopPanel), true);

  ImDrawList* draw_list = ImGui::GetWindowDrawList();
  ImVec2 offset = ImGui::GetCursorScreenPos() - this->scrolling;

  draw_list->ChannelsSetCurrent(0);
  ImVec2 bb_min = offset;
  ImVec2 bb_max = ImVec2(this->textureWidth, this->textureHeight) + offset;
  draw_list->AddImage(ImTextureID(intptr_t(this->vboTexture)), bb_min, bb_max);

  draw_list->ChannelsMerge();

  ImGui::EndChild();
  // END preview

  // BEGIN preview delimiter
  ImGui::GetIO().MouseDrawCursor = true;
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(89, 91, 94)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(119, 122, 124)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0)));
  ImGui::Button("###splitterGUI", ImVec2(-1, 6.0f));
  ImGui::PopStyleColor(3);
  if (ImGui::IsItemActive())
    this->heightTopPanel += ImGui::GetIO().MouseDelta.y;
  if (ImGui::IsItemHovered())
    ImGui::SetMouseCursor(3);
  else
    ImGui::GetIO().MouseDrawCursor = false;
  // END preview delimiter

  // BEGIN editor
  ImGui::BeginChild("Editor", ImVec2(0, 0), false);

  // buttons
  if (ImGui::Button("COMPILE", ImVec2(ImGui::GetWindowWidth() * 0.85f, this->buttonCompileHeight)))
    this->compileShader();
  ImGui::SameLine();
  if (ImGui::Button("Paste", ImVec2(ImGui::GetWindowWidth() * 0.14f, this->buttonCompileHeight)))
    this->getFromClipboard();

  // BEGIN textures
  ImGui::BeginChild("Options", ImVec2(this->widthTexturesPanel, 0), false);

  ImGui::Text("Examples");

  if (ImGui::Button("Artificial", ImVec2(-1.0f, 0.0f)))
    this->openExample(Settings::Instance()->appFolder() + "/shaders/stoy/4ljGW1.stoy");
  if (ImGui::IsItemHovered()) ImGui::SetTooltip("Artificial");

  if (ImGui::Button(" Combustible\nVoronoi Layers", ImVec2(-1.0f, 0.0f)))
    this->openExample(Settings::Instance()->appFolder() + "/shaders/stoy/4tlSzl.stoy");
  if (ImGui::IsItemHovered()) ImGui::SetTooltip("Combustible Voronoi Layers");

  if (ImGui::Button("Seascape", ImVec2(-1.0f, 0.0f)))
    this->openExample(Settings::Instance()->appFolder() + "/shaders/stoy/Ms2SD1.stoy");
  if (ImGui::IsItemHovered()) ImGui::SetTooltip("Seascape");

  if (ImGui::Button("Star Nest", ImVec2(-1.0f, 0.0f)))
    this->openExample(Settings::Instance()->appFolder() + "/shaders/stoy/XlfGRj.stoy");
  if (ImGui::IsItemHovered()) ImGui::SetTooltip("Star Nest");

  if (ImGui::Button("Sun Surface", ImVec2(-1.0f, 0.0f)))
    this->openExample(Settings::Instance()->appFolder() + "/shaders/stoy/XlSSzK.stoy");
  if (ImGui::IsItemHovered()) ImGui::SetTooltip("Sun Surface");

  ImGui::Separator();

  const char* textureImages[] = {
    " -- NONE -- ",
    "tex00.jpg", "tex01.jpg", "tex02.jpg", "tex03.jpg", "tex04.jpg", "tex05.jpg", "tex06.jpg", "tex07.jpg", "tex08.jpg", "tex09.jpg", "tex10.jpg",
    "tex11.jpg", "tex12.jpg", "tex13.jpg", "tex14.jpg", "tex15.jpg", "tex16.jpg", "tex17.jpg", "tex18.jpg", "tex19.jpg", "tex20.jpg"
  };

  const char* cubemapImages[] = {
    " -- NONE -- ",
    "cube00_0.jpg", "cube00_1.jpg", "cube00_2.jpg", "cube00_3.jpg",
    "cube00_4.jpg", "cube00_5.jpg", "cube01_0.png", "cube01_1.png", "cube01_2.png", "cube01_3.png", "cube01_4.png", "cube01_5.png",
    "cube02_0.jpg", "cube02_1.jpg", "cube02_2.jpg", "cube02_3.jpg", "cube02_4.jpg", "cube02_5.jpg", "cube03_0.png", "cube03_1.png",
    "cube03_2.png", "cube03_3.png", "cube03_4.png", "cube03_5.png", "cube04_0.png", "cube04_1.png", "cube04_2.png", "cube04_3.png",
    "cube04_4.png", "cube04_5.png", "cube05_0.png", "cube05_1.png", "cube05_2.png", "cube05_3.png", "cube05_4.png", "cube05_5.png"
  };

  ImGui::PushItemWidth(-1);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 100));

  ImGui::TextColored(ImColor(255.0f, 0.0f, 0.0f, 255.0f), "Channel #0");
  ImGui::Checkbox("Cubemap?##001", &this->channel0Cube);
  if (this->channel0Cube)
    ImGui::ListBox("##cubemap0", &this->cubemapImage0, cubemapImages, IM_ARRAYSIZE(cubemapImages), 3);
  else
    ImGui::ListBox("##texImage0", &this->texImage0, textureImages, IM_ARRAYSIZE(textureImages), 3);

  ImGui::Separator();

  ImGui::TextColored(ImColor(255.0f, 0.0f, 0.0f, 255.0f), "Channel #1");
  ImGui::Checkbox("Cubemap?##002", &this->channel1Cube);
  if (this->channel1Cube)
    ImGui::ListBox("##cubemap1", &this->cubemapImage1, cubemapImages, IM_ARRAYSIZE(cubemapImages), 3);
  else
    ImGui::ListBox("##texImage1", &this->texImage1, textureImages, IM_ARRAYSIZE(textureImages), 3);

  ImGui::Separator();

  ImGui::TextColored(ImColor(255.0f, 0.0f, 0.0f, 255.0f), "Channel #2");
  ImGui::Checkbox("Cubemap?##003", &this->channel2Cube);
  if (this->channel2Cube)
    ImGui::ListBox("##cubemap2", &this->cubemapImage2, cubemapImages, IM_ARRAYSIZE(cubemapImages), 3);
  else
    ImGui::ListBox("##texImage2", &this->texImage2, textureImages, IM_ARRAYSIZE(textureImages), 3);

  ImGui::Separator();

  ImGui::TextColored(ImColor(255.0f, 0.0f, 0.0f, 255.0f), "Channel #3");
  ImGui::Checkbox("Cubemap?##004", &this->channel3Cube);
  if (this->channel3Cube)
    ImGui::ListBox("##cubemap3", &this->cubemapImage3, cubemapImages, IM_ARRAYSIZE(cubemapImages), 3);
  else
    ImGui::ListBox("##texImage3", &this->texImage3, textureImages, IM_ARRAYSIZE(textureImages), 3);

  ImGui::PopStyleVar(2);

  if (this->texImage0 > 0) {
    this->engineShadertoy->iChannel0_CubeImage.clear();
    this->engineShadertoy->iChannel0_Image = Settings::Instance()->appFolder() + "tex" + std::string((this->texImage0 < 10) ? "0" : "") + std::to_string(this->texImage0) + ".jpg";
    this->texImage0 = 0;
  }
  if (this->cubemapImage0 > 0) {
    this->engineShadertoy->iChannel0_Image.clear();
    this->engineShadertoy->iChannel0_CubeImage = Settings::Instance()->appFolder() + cubemapImages[this->cubemapImage0];
    this->cubemapImage0 = 0;
  }

  if (this->texImage1 > 0) {
    this->engineShadertoy->iChannel1_CubeImage.clear();
    this->engineShadertoy->iChannel1_Image = Settings::Instance()->appFolder() + "tex" + std::string((this->texImage1 < 10) ? "0" : "") + std::to_string(this->texImage1) + ".jpg";
    this->texImage1 = 0;
  }
  if (this->cubemapImage1 > 0) {
    this->engineShadertoy->iChannel1_Image.clear();
    this->engineShadertoy->iChannel1_CubeImage = Settings::Instance()->appFolder() + cubemapImages[this->cubemapImage1];
    this->cubemapImage1 = 0;
  }

  if (this->texImage2 > 0) {
    this->engineShadertoy->iChannel2_CubeImage.clear();
    this->engineShadertoy->iChannel2_Image = Settings::Instance()->appFolder() + "tex" + std::string((this->texImage2 < 10) ? "0" : "") + std::to_string(this->texImage2) + ".jpg";
    this->texImage2 = 0;
  }
  if (this->cubemapImage2 > 0) {
    this->engineShadertoy->iChannel2_Image.clear();
    this->engineShadertoy->iChannel2_CubeImage = Settings::Instance()->appFolder() + cubemapImages[this->cubemapImage2];
    this->cubemapImage2 = 0;
  }

  if (this->texImage3 > 0) {
    this->engineShadertoy->iChannel3_CubeImage.clear();
    this->engineShadertoy->iChannel3_Image = Settings::Instance()->appFolder() + "tex" + std::string((this->texImage3 < 10) ? "0" : "") + std::to_string(this->texImage3) + ".jpg";
    this->texImage3 = 0;
  }
  if (this->cubemapImage3 > 0) {
    this->engineShadertoy->iChannel3_Image.clear();
    this->engineShadertoy->iChannel3_CubeImage = Settings::Instance()->appFolder() + cubemapImages[this->cubemapImage3];
    this->cubemapImage3 = 0;
  }

  ImGui::EndChild();

  // BEGIN textures delimiter
  ImGui::SameLine();
  ImGui::GetIO().MouseDrawCursor = true;
  ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor(89, 91, 94)));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor(119, 122, 124)));
  ImGui::PushStyleColor(ImGuiCol_Border, static_cast<ImVec4>(ImColor(0, 0, 0)));
  ImGui::Button("###splitterGUI2", ImVec2(4.0f, -1));
  ImGui::PopStyleColor(3);
  if (ImGui::IsItemActive())
    this->widthTexturesPanel += ImGui::GetIO().MouseDelta.x;
  if (ImGui::IsItemHovered())
    ImGui::SetMouseCursor(4);
  else
    ImGui::GetIO().MouseDrawCursor = false;
  ImGui::SameLine();
  // END textures delimiter

  // BEGIN IDE
  ImGui::BeginChild("IDE", ImVec2(0.0f, 0.0f), false);
  int lines = static_cast<int>((ImGui::GetWindowHeight() - 4.0f) / ImGui::GetTextLineHeight());
  ImGui::InputTextMultiline("##source", this->shadertoyEditorText, IM_ARRAYSIZE(this->shadertoyEditorText), ImVec2(-1.0f, ImGui::GetTextLineHeight() * lines), ImGuiInputTextFlags_AllowTabInput);
  ImGui::EndChild();

  // END editor
  ImGui::EndChild();

  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
    this->scrolling = this->scrolling - ImGui::GetIO().MouseDelta;

  ImGui::End();
}

void DialogShadertoy::init(const std::function<void()>& fShowErrorMessage) {
  this->funcShowMessage = fShowErrorMessage;
  this->viewPaddingHorizontal = 20.0f;
  this->viewPaddingVertical = 40.0f;
  this->heightTopPanel = 200.0f;
  this->widthTexturesPanel = 140.0f;
  this->buttonCompileHeight = 44.0f;
  this->textureWidth = int(this->windowWidth - this->viewPaddingHorizontal);
  this->textureHeight = int(this->windowHeight - this->viewPaddingVertical);

  this->channel0Cube = false;
  this->channel1Cube = false;
  this->channel2Cube = false;
  this->channel3Cube = false;
  this->cubemapImage0 = -1;
  this->cubemapImage1 = -1;
  this->cubemapImage2 = -1;
  this->cubemapImage3 = -1;
  this->texImage0 = -1;
  this->texImage1 = -1;
  this->texImage2 = -1;
  this->texImage3 = -1;

  const char* funcMain = "void mainImage(out vec4 fragColor, in vec2 fragCoord)\n"
                          "{\n"
                          "   vec2 uv = fragCoord.xy / iResolution.xy;\n"
                          "   fragColor = vec4(uv, 0.5 + 0.5 * sin(iGlobalTime), 1.0);\n"
                          "}\n\0";
  strcpy(this->shadertoyEditorText, funcMain);

  this->engineShadertoy = std::make_unique<Shadertoy>();
  this->engineShadertoy->initShaderProgram(std::string(funcMain));
  this->engineShadertoy->initBuffers();
  this->engineShadertoy->initFBO(
              Settings::Instance()->SDL_Window_Width,
              Settings::Instance()->SDL_Window_Height,
              &this->vboTexture);
}

void DialogShadertoy::compileShader() {
  this->engineShadertoy->initShaderProgram(std::string(this->shadertoyEditorText));
  this->engineShadertoy->initBuffers();
  this->engineShadertoy->initFBO(
              int(this->windowWidth),
              int(this->heightTopPanel),
              &this->vboTexture);
}

std::string DialogShadertoy::exec(const char* cmd) const {
#ifdef _WIN32
  return "";
#else
  FILE* pipe = popen(cmd, "r");
  if (!pipe)
    return "ERROR";

  char buffer[128];
  std::string result("");
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  return result;
#endif
}

std::string DialogShadertoy::paste() const {
#ifdef _WIN32
  return "";
#else
  return this->exec("pbpaste");
#endif
}

void DialogShadertoy::getFromClipboard() {
  std::string clipboardContents = this->paste();
  if (clipboardContents.length() > (1024 * 16))
    this->funcShowMessage();
  else
    strcpy(this->shadertoyEditorText, clipboardContents.c_str());
}

void DialogShadertoy::openExample(const std::string& fileName) {
  std::string fileContents = Settings::Instance()->glUtils->readFile(fileName.c_str());
  strcpy(this->shadertoyEditorText, fileContents.c_str());
  //this->compileShader();
}
