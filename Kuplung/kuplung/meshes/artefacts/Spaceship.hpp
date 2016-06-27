//
//  Spaceship.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Spaceship_hpp
#define Spaceship_hpp

#include <glm/glm.hpp>
#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/pcg/SpaceshipMeshGenerator.hpp"

class Spaceship {
public:
    ~Spaceship();
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers(int gridSize);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel);
    SpaceshipMeshGenerator *spaceshipGenerator;

    bool Setting_UseTexture, Setting_Wireframe;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboTextureCoordinates, vboTextureDiffuse, vboNormals, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glAttributeColor;
    GLint glAttributeTextureCoord, glUniformHasTexture, glUniformSamplerTexture;

    std::string readFile(const char *filePath);
};

#endif /* Spaceship_hpp */
