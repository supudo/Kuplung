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

struct GUIObjectSetting {
    int oIndex;
    bool oAnimate;
    int iValue;
    float fValue;
    bool bValue;
    glm::vec4 vValue;
};

typedef enum LightType {
    LightType_Point = 0,
    LightType_Sun = 1
} LightType;

struct LightObject {
    bool colorPickerOpen, doAnimation;
    float strength;
    glm::vec3 color;
};

struct SceneLightCoordinate {
    bool doAnimation;
    float coordinate;
};

struct SceneLight {
    std::string lightTitle;
    LightType lightType;
    SceneLightCoordinate *positionX, *positionY, *positionZ;
    SceneLightCoordinate *directionX, *directionY, *directionZ;
    LightObject *ambient, *diffuse, *specular;
};

#endif /* Objects_hpp */
