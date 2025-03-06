//
//  ImageViewer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "ImageViewer.hpp"
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/stb/stb_image.h"
#include <filesystem>

void ImageViewer::showImage(bool* show) {
  if (this->genTexture)
    this->createTextureBuffer();

  int winWidth = this->tWidth;
  if (this->tWidth > this->wWidth)
    winWidth = this->wWidth - 80;

  int wintHeight = this->tHeight;
  if (this->tHeight > this->wHeight)
    wintHeight = this->wHeight - 80;

  ImGui::SetNextWindowSize(ImVec2(winWidth, wintHeight), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(40.0f, 40.0f), ImGuiCond_FirstUseEver);

  /// MIGRATE : ImGui::Begin("Image Viewer", show, ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_HorizontalScrollbar);
  ImGui::Begin("Image Viewer", show);

  ImGui::Text("Image: %s", imagePath.c_str());
  ImGui::Text("Image dimensions: %i x %i", this->tWidth, this->tHeight);

  ImGui::Separator();

  ImGui::Image(ImTextureID(intptr_t(this->vboBuffer)), ImVec2(this->tWidth, this->tHeight));

  ImGui::End();
  this->genTexture = false;
}

void ImageViewer::createTextureBuffer() {
  if (!std::filesystem::exists(this->imagePath))
    this->imagePath = Settings::Instance()->currentFolder + "/" + this->imagePath;
  int tChannels;
  unsigned char* tPixels = stbi_load(this->imagePath.c_str(), &this->tWidth, &this->tHeight, &tChannels, 0);
  if (!tPixels)
    Settings::Instance()->funcDoLog("[Kuplung-ImageViewer] Can't load texture image - " + this->imagePath + " with error - " + std::string(stbi_failure_reason()));
  else {
    glGenTextures(1, &this->vboBuffer);
    glBindTexture(GL_TEXTURE_2D, this->vboBuffer);
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
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(textureFormat), this->tWidth, this->tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, (GLvoid*)tPixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tPixels);
    this->tWidth += 20;
    this->tHeight += 20;
  }
}
