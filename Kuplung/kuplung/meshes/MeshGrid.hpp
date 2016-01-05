//
//  MeshGrid.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MeshGrid_hpp
#define MeshGrid_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/parsers/obj-parser/objObjects.h"
#include "utilities/gl/GLUtils.hpp"

class MeshGrid {
public:
    ~MeshGrid();
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

#endif /* MeshGrid_hpp */
