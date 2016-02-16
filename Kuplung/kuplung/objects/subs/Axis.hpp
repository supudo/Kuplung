//
//  Axis.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Axis_hpp
#define Axis_hpp

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/CoordinateSystem.hpp"

class Axis {
public:
    Axis(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description);

    void destroy();
    void initProperties();
    void render(glm::mat4 mtxProjection, glm::mat4 mtxCamera);

    std::string title;
    std::string description;

    ObjectEye *eyeSettings;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> funcLog;

    CoordinateSystem *axisSystem;

    void logMessage(std::string message);
};

#endif /* Axis_hpp */
