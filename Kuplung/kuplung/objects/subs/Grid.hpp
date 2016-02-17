//
//  Grid.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Grid_hpp
#define Grid_hpp

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/meshes/WorldGrid.hpp"

class Grid {
public:
    Grid(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description);

    void destroy();
    void initProperties(int size);
    void reinitBuffers(int gs);
    void render(glm::mat4 mtxProjection, glm::mat4 mtxCamera);

    std::string title;
    std::string description;
    int gridSize;

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> funcLog;

    WorldGrid* sceneGridHorizontal;
    WorldGrid* sceneGridVertical;

    void logMessage(std::string message);
};

#endif /* Grid_hpp */
