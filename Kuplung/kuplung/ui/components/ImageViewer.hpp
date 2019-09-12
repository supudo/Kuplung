//
//  ImageViewer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef ImageViewer_hpp
#define ImageViewer_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/imgui/imgui.h"

class ImageViewer {
public:
  void showImage(bool* show);
  bool genTexture;
  std::string imagePath;
  int wWidth, wHeight;

private:
  GLuint vboBuffer;
  int tWidth, tHeight;
  void createTextureBuffer();

};

#endif /* ImageViewer_hpp */
