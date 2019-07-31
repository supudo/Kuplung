//
//  ObjectDefinitions.h
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ObjectDefinitions_h
#define ObjectDefinitions_h

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "kuplung/utilities/gl/GLIncludes.h"

typedef enum ViewModelSkin {
  ViewModelSkin_Solid,
  ViewModelSkin_Material,
  ViewModelSkin_Texture,
  ViewModelSkin_Wireframe,
  ViewModelSkin_Rendered
} ViewModelSkin;

typedef enum LightSourceType {
  LightSourceType_Directional,
  LightSourceType_Point,
  LightSourceType_Spot
} LightSourceType;

typedef enum MaterialTextureType {
  MaterialTextureType_Undefined,
  MaterialTextureType_Ambient,
  MaterialTextureType_Diffuse,
  MaterialTextureType_Dissolve,
  MaterialTextureType_Bump,
  MaterialTextureType_Specular,
  MaterialTextureType_SpecularExp,
  MaterialTextureType_Displacement
} MaterialTextureType;

struct PixelDataPoint {
  GLbyte color[4];
  GLuint index;
  GLfloat depth;
};

struct ObjectCoordinate {
  bool animate;
  float point;
  ObjectCoordinate() : animate(false), point(0.0f) { }
  ObjectCoordinate(bool a, float p) : animate(a), point(p) { }
};

struct ObjectEye {
  glm::vec3 View_Eye;
  glm::vec3 View_Center;
  glm::vec3 View_Up;
};

struct MaterialColor {
  bool colorPickerOpen;
  bool animate;
  float strength;
  glm::vec3 color;
  MaterialColor() : colorPickerOpen(false), animate(false), strength(0.0f), color(glm::vec3(1, 1, 1)) {}
  MaterialColor(bool cpo, bool a, float s, glm::vec3 c) : colorPickerOpen(cpo), animate(a), strength(s), color(c) {}
};

struct Skybox_Item {
  std::string title;
  std::vector<std::string> images;
};

std::string static Kuplung_getTextureName(MaterialTextureType texType) {
  switch (texType) {
    case MaterialTextureType_Ambient:
      return "Ambient";
    case MaterialTextureType_Diffuse:
      return "Diffuse";
    case MaterialTextureType_Dissolve:
      return "Dissolve";
    case MaterialTextureType_Bump:
      return "Normal";
    case MaterialTextureType_Specular:
      return "Specular";
    case MaterialTextureType_SpecularExp:
      return "Specular Exp";
    case MaterialTextureType_Displacement:
      return "Displacement";
    default:
      return "";
  }
}

#endif /* ObjectDefinitions_h */
