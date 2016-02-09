//
//  Dot.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Dot_hpp
#define Dot_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/parsers/obj-parser/objObjects.h"
#include "utilities/gl/GLUtils.hpp"

class Dot {
public:
    ~Dot();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    bool initShaderProgram();
    void initBuffers(glm::vec3 position, glm::vec3 direction, bool simple);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);

private:
    std::function<void(std::string)> doLogFunc;
    int glslVersion;
    std::string shaderName;
    int axisSize;
    float x, y, z;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO, vboVertices, vboIndices;
    GLuint glAttributeVertexPosition, glUniformMVPMatrix;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* Dot_hpp */
