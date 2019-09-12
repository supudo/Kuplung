//
//  DialogSVS.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogSVS_hpp
#define DialogSVS_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/imgui/imgui.h"
#include "kuplung/meshes/artefacts/StructuredVolumetricSampling.hpp"

class DialogSVS {
public:
  void init();
  void render(bool* p_opened);

  GLuint vboTexture;
  float windowWidth, windowHeight;
  float viewPaddingHorizontal, viewPaddingVertical;

private:
  int textureWidth, textureHeight;
  ImVec2 scrolling = ImVec2(0.0f, 0.0f);

  std::unique_ptr<StructuredVolumetricSampling> structured_Volumetric_Sampling;
};

#endif /* DialogSVS_hpp */
