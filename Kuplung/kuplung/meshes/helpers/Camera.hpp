//
//  Camera.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <string>
#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectDefinitions.h"

class Camera {
public:
    ~Camera();
    Camera();
    void initProperties();
    void render();
    glm::vec3 createRay(const float mouse_x, const float mouse_y, const float fov, const float ratio, const float near, const float far);
    PixelDataPoint getClickData(const int x, const int y, const int height);

    std::unique_ptr<ObjectEye> eyeSettings;
    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
    std::unique_ptr<ObjectCoordinate> rotateCenterX, rotateCenterY, rotateCenterZ;

    glm::vec3 cameraPosition;
    glm::mat4 matrixCamera;
};

#endif /* Camera_hpp */
