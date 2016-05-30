//
//  Terrain.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Terrain_hpp
#define Terrain_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/terrain/HeightmapGenerator.hpp"

class Terrain {
public:
    ~Terrain();
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    std::string heightmapImage;
    HeightmapGenerator *terrainGenerator;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeColor;

    std::string readFile(const char *filePath);
};

#endif /* Terrain_hpp */
