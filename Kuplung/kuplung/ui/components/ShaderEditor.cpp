//
//  ShaderEditor.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/25/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/ShaderEditor.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include <fstream>

const char* GUIEditor_ShaderItems[] = {
  "-- Select app shader --",
  "kuplung.vert",
  "kuplung.frag",
  "kuplung.geom",
  "light.vert",
  "light.frag",
  "grid.vert",
  "grid.frag",
  "axis.vert",
  "axis.frag",
  "dots.vert",
  "dots.frag",
  "terrain.vert",
  "terrain.frag"
};

void ShaderEditor::init(std::string const& appPath, int positionX, int positionY, int width, int height) {
  this->appPath = appPath;
  this->positionX = positionX;
  this->positionY = positionY;
  this->width = width;
  this->height = height;
  this->shaderFileIndex = 0;
  this->currentFileName.clear();

  this->managerLua = std::make_unique<KuplungApp::Utilities::Lua::LuaManager>();
  this->managerLua->initLua();
  std::string luaFile = Settings::Instance()->appFolder() + "/lua/test.lua";
  this->managerLua->execute(luaFile);
}

void ShaderEditor::draw(const std::function<void(std::string)>& fileShaderCompile, const char* title, bool* p_opened) {
  this->doFileShaderCompile = fileShaderCompile;

  if (this->width > 0 && this->height > 0)
    ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
  else
    ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameFileBrowser_Width, Settings::Instance()->frameFileBrowser_Height), ImGuiSetCond_FirstUseEver);

  if (this->positionX > 0 && this->positionY > 0)
    ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

  /// MIGRATE : ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders);
  ImGui::Begin(title, p_opened);

  ImGui::Combo("Shader##", &this->shaderFileIndex, GUIEditor_ShaderItems, IM_ARRAYSIZE(GUIEditor_ShaderItems));
  ImGui::SameLine();
  if (ImGui::Button("Compile"))
    this->compileShader();
  ImGui::Separator();

  ImGui::BeginChild("scrolling");
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));

  if (this->shaderFileIndex > 0) {
    if (this->currentFileName != GUIEditor_ShaderItems[this->shaderFileIndex]) {
      this->currentFileName = GUIEditor_ShaderItems[this->shaderFileIndex];
      std::string filePath = Settings::Instance()->appFolder() + "/shaders/" + GUIEditor_ShaderItems[this->shaderFileIndex];
      std::FILE* fp = std::fopen(filePath.c_str(), "rb");
      if (fp) {
        std::fseek(fp, 0, SEEK_END);
        this->fileContents.resize(std::ftell(fp));
        std::rewind(fp);
        std::fread(&this->fileContents[0], 1, this->fileContents.size(), fp);
        std::fclose(fp);
        if (!this->fileContents.empty())
          strcpy(this->guiEditorText, this->fileContents.c_str());
      }
    }
  }
  else
    this->fileContents.clear();

  int lines = ImGui::GetWindowHeight() / ImGui::GetTextLineHeight();
  ImGui::InputTextMultiline("##source", this->guiEditorText, IM_ARRAYSIZE(this->guiEditorText), ImVec2(-1.0f, ImGui::GetTextLineHeight() * lines), ImGuiInputTextFlags_AllowTabInput);

  ImGui::PopStyleVar();

  ImGui::Separator();
  ImGui::Spacing();

  ImGui::EndChild();
  ImGui::End();
}

void ShaderEditor::compileShader() {
  if (this->shaderFileIndex > 0) {
    std::string shaderSource = std::string(this->guiEditorText);
    if (!shaderSource.empty()) {
      std::string shaderFile = Settings::Instance()->appFolder() + "/shaders/" + GUIEditor_ShaderItems[this->shaderFileIndex];
      std::ofstream fileWrite(shaderFile);
      fileWrite << shaderSource;
      fileWrite.close();
      this->doFileShaderCompile(GUIEditor_ShaderItems[this->shaderFileIndex]);
    }
  }
}
