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
#include "kuplung/utilities/gl/GLUtils.hpp"

class WorldGrid {
public:
    ~WorldGrid();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    bool initShaderProgram();
    void initBuffers(int gridSize, bool isHorizontal, float unitSize);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    int gridSize;

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
