//
//  CoordinateSystem.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef CoordinateSystem_hpp
#define CoordinateSystem_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class CoordinateSystem {
public:
    ~CoordinateSystem();
    void destroy();
    void init(std::function<void(std::string)> doLog);
    void initProperties();
    bool initShaderProgram();
    void initBuffers(); // 0 = x, 1 = y, 2 = z
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera);

    bool showAxis;

    ObjectEye *eyeSettings;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> doLogFunc;
    int axisSize;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboColors;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix, glAttributeColor;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* CoordinateSystem_hpp */
