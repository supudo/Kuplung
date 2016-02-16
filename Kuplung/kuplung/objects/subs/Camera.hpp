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
#include <functional>
#include <string>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"

class Camera {
public:
    Camera(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description);

    void destroy();
    void initProperties();
    void render(float Setting_PlaneClose, float Setting_PlaneFar);

    std::string title;
    std::string description;

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::vec3 cameraPosition;
    glm::mat4 matrixCamera;

private:
    std::function<void(std::string)> funcLog;

    void logMessage(std::string message);
};

#endif /* Camera_hpp */
