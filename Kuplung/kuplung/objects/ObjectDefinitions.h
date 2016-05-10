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
#include "kuplung/utilities/gl/GLIncludes.h"

typedef enum ViewModelSkin {
    ViewModelSkin_Solid,
    ViewModelSkin_Material,
    ViewModelSkin_Texture,
    ViewModelSkin_Wireframe
} ViewModelSkin;

typedef enum LightSourceType {
    LightSourceType_Directional,
    LightSourceType_Point,
    LightSourceType_Spot
} LightSourceType;

typedef enum MaterialTextureType {
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
    GLfloat depth;
    GLuint index;
};

struct ObjectCoordinate {
    bool animate;
    float point;
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
};

struct Skybox_Item {
    std::string title;
    std::vector<std::string> images;
};

std::string static Kuplung_getTextureName(MaterialTextureType texType) {
    std::string texName = "";
    switch (texType) {
        case MaterialTextureType_Ambient:
            texName = "Ambient";
            break;
        case MaterialTextureType_Diffuse:
            texName = "Diffuse";
            break;
        case MaterialTextureType_Dissolve:
            texName = "Dissolve";
            break;
        case MaterialTextureType_Bump:
            texName = "Normal";
            break;
        case MaterialTextureType_Specular:
            texName = "Specular";
            break;
        case MaterialTextureType_SpecularExp:
            texName = "Specular Exp";
            break;
        case MaterialTextureType_Displacement:
            texName = "Displacement";
            break;
        default:
            break;
    }
    return texName;
}

#endif /* ObjectDefinitions_h */
