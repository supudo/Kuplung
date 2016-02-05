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

struct GUILightObject {
    bool colorPickerOpen;
    float strength;
    glm::vec3 color;
};

struct GUISceneLight {
    GUILightObject *ambient;
    GUILightObject *diffuse;
    GUILightObject *specular;
};

#endif /* Objects_hpp */
