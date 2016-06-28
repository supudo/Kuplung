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
#include "kuplung/ui/Objects.h"
#include "kuplung/meshes/helpers/Light.hpp"

class Spaceship {
public:
    ~Spaceship();
    void destroy();
    void init();
    bool initShaderProgram();
    void initBuffers(int gridSize);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, Light* light);
    SpaceshipMeshGenerator *spaceshipGenerator;

    bool Setting_UseTexture, Setting_Wireframe;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboTextureCoordinates, vboTextureDiffuse, vboNormals, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glAttributeColor, glFS_CameraPosition;
    GLint glAttributeTextureCoord, glUniformHasTexture, glUniformSamplerTexture;

    ModelFace_LightSource_Directional *lightSource;

    std::string readFile(const char *filePath);
};

#endif /* Spaceship_hpp */
