//
//  MeshCoordinateSystem.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MeshCoordinateSystem_hpp
#define MeshCoordinateSystem_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/parsers/obj-parser/objObjects.h"
#include "utilities/gl/GLUtils.hpp"

class MeshCoordinateSystem {
public:
    ~MeshCoordinateSystem();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    bool initShaderProgram();
    void initBuffers(); // 0 = x, 1 = y, 2 = z
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);

private:
    std::function<void(std::string)> doLogFunc;
    int glslVersion;
    std::string shaderName;
    int axisSize;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboColors;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix, glAttributeColor;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* MeshCoordinateSystem_hpp */
