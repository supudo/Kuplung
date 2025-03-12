//
//  DialogShadertoy.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogShadertoy_hpp
#define DialogShadertoy_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include <imgui.h>
#include "kuplung/meshes/artefacts/Shadertoy.hpp"

class DialogShadertoy {
public:
  void init(const std::function<void()>& fShowErrorMessage);
  void render(bool* p_opened);

  GLuint vboTexture;
  float windowWidth, windowHeight;
  float viewPaddingHorizontal, viewPaddingVertical;

private:
  std::function<void()> funcShowMessage;

  int textureWidth, textureHeight;
  ImVec2 scrolling = ImVec2(0.0f, 0.0f);

  std::unique_ptr<Shadertoy> engineShadertoy;
  float heightTopPanel, widthTexturesPanel, buttonCompileHeight;
  char shadertoyEditorText[1024 * 16];
  void compileShader();
  bool channel0Cube, channel1Cube, channel2Cube, channel3Cube;
  int texImage0, texImage1, texImage2, texImage3;
  int cubemapImage0, cubemapImage1, cubemapImage2, cubemapImage3;

  std::string exec(const char* cmd) const;
  std::string paste() const;
  void getFromClipboard();
  void openExample(const std::string& fileName);
};

#endif /* DialogShadertoy_hpp */
