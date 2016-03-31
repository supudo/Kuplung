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

typedef enum LightSourceType {
    LightSourceType_Directional,
    LightSourceType_Point,
    LightSourceType_Spot
} LightSourceType;

struct MaterialColor {
    bool colorPickerOpen;
    bool animate;
    float strength;
    glm::vec3 color;
};

#endif /* ObjectDefinitions_h */
