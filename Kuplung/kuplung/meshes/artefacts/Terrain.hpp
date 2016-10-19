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
#include "kuplung/pcg/HeightmapGenerator.hpp"

class Terrain {
public:
    ~Terrain();
    void init();
    bool initShaderProgram();
    void initBuffers(std::string const& assetsFolder, int width, int height);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    std::string heightmapImage;
    std::unique_ptr<HeightmapGenerator> terrainGenerator;

    bool Setting_UseTexture, Setting_Wireframe;

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboTextureCoordinates, vboTextureDiffuse, vboNormals, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glAttributeColor;
    GLint glAttributeTextureCoord, glUniformHasTexture, glUniformSamplerTexture;
};

#endif /* Terrain_hpp */
