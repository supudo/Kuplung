//
//  Terrain.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "utilities/gl/GLIncludes.h"
#include "utilities/gl/GLUtils.hpp"
#include "utilities/libnoise/HeightmapGenerator.hpp"

class Terrain {
public:
    ~Terrain();
    void destroy();
    void init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion);
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    std::string heightmapImage;

private:
    std::function<void(std::string)> doLogFunc;
    int glslVersion;
    std::string shaderName;

    GLUtils *glUtils;
    HeightmapGenerator *terrainGenerator;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeColor;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* Terrain_hpp */
