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
    void destroy();
    void initProperties();
    void render();
    glm::vec3 createRay(float mouse_x, float mouse_y, float fov, float ratio, float near, float far);
    PixelDataPoint getClickData(int x, int y, int height);

    std::unique_ptr<ObjectEye> eyeSettings;
    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
    std::unique_ptr<ObjectCoordinate> rotateCenterX, rotateCenterY, rotateCenterZ;

    glm::vec3 cameraPosition;
    glm::mat4 matrixCamera;
};

#endif /* Camera_hpp */
