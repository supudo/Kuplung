//
//  Objects.h
//  Kuplung
//
//  Created by Sergey Petrov on 11/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Objects_hpp
#define Objects_hpp

#include <glm/glm.hpp>
#include <string>
#include "kuplung/utilities/gl/GLIncludes.h"

struct GUIObjectSetting {
    int oIndex, iValue;
    bool oAnimate, bValue;
    float fValue;
    glm::vec4 vValue;
};

typedef enum LightType {
    LightType_Point = 0,
    LightType_Sun = 1
} LightType;

struct LightObject {
    float strength;
    bool colorPickerOpen, doAnimation;
    glm::vec3 color;
};

struct SceneLightCoordinate {
    float coordinate;
    bool doAnimation;
};

struct SceneLight {
    std::string lightTitle;
    LightType lightType;
    SceneLightCoordinate *positionX, *positionY, *positionZ;
    SceneLightCoordinate *directionX, *directionY, *directionZ;
    LightObject *ambient, *diffuse, *specular;
};

struct ModelFace_LightSource_Directional {
    GLint gl_InUse;
    GLint gl_Direction;
    GLint gl_Ambient, gl_Diffuse, gl_Specular;
    GLint gl_StrengthAmbient, gl_StrengthDiffuse, gl_StrengthSpecular;
};

struct ModelFace_LightSource_Point {
    GLint gl_InUse;
    GLint gl_Position;
    GLint gl_Constant, gl_Linear, gl_Quadratic;
    GLint gl_Ambient, gl_Diffuse, gl_Specular;
    GLint gl_StrengthAmbient, gl_StrengthDiffuse, gl_StrengthSpecular;
};

struct ModelFace_LightSource_Spot {
    GLint gl_InUse;
    GLint gl_Position, gl_Direction;
    GLint gl_CutOff, gl_OuterCutOff;
    GLint gl_Constant, gl_Linear, gl_Quadratic;
    GLint gl_Ambient, gl_Diffuse, gl_Specular;
    GLint gl_StrengthAmbient, gl_StrengthDiffuse, gl_StrengthSpecular;
};

struct RenderingForward_LightSource {
    GLint gl_Position, gl_Color, gl_Linear, gl_Quadratic, gl_Radius;
};

#endif /* Objects_hpp */
