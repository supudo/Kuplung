//
//  WorldGrid.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef WorldGrid_hpp
#define WorldGrid_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class WorldGrid {
public:
    ~WorldGrid();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    bool initShaderProgram();
    void initBuffers(int gridSize, float unitSize);
    void initProperties(int size);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera);
    int gridSize;

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> doLogFunc;
    int glslVersion;
    std::string shaderName;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* WorldGrid_hpp */
