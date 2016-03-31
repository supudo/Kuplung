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
    void destroy();
    void initProperties();
    void render(float Setting_PlaneClose, float Setting_PlaneFar);
    glm::vec3 createRay(float mouse_x, float mouse_y, float fov, float ratio, float near, float far);
    PixelDataPoint getClickData(int x, int y, int height);

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::vec3 cameraPosition;
    glm::mat4 matrixCamera;
};

#endif /* Camera_hpp */
