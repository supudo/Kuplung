//
//  LightSource.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef LightSource_hpp
#define LightSource_hpp

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/Light.hpp"
#include "kuplung/meshes/LightRay.hpp"

class LightSource {
public:
    LightSource(std::function<void(std::string)> doLog, objScene obj, LightSourceType type, std::string title, std::string description);

    void destroy();
    void initProperties();
    void initModels();
    void render(glm::mat4 mtxProjection, glm::mat4 mtxCamera, glm::mat4 mtxGrid, bool fixedGridWorld);

    std::string title;
    std::string description;
    LightSourceType type;

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *directionX, *directionY, *directionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;
    MaterialColor *ambient, *diffuse, *specular;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> funcLog;

    void logMessage(std::string message);

    objScene objModel;
    Light* meshLight;
    LightRay* meshLightRay;
};

#endif /* LightSource_hpp */
