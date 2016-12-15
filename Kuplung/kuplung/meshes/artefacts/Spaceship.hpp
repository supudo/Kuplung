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
#include "kuplung/pcg/SpaceshipMeshGenerator.hpp"
#include "kuplung/ui/Objects.h"
#include "kuplung/meshes/helpers/Light.hpp"

class Spaceship {
public:
    ~Spaceship();
    Spaceship();
    bool initShaderProgram();
    void initBuffers(const int gridSize);
    void render(const glm::mat4 matrixProjection, const glm::mat4 matrixCamera, const glm::mat4 matrixModel, const glm::vec3 vecCameraPosition);
    std::unique_ptr<SpaceshipMeshGenerator> spaceshipGenerator;

    float solidLightSkin_Ambient_Strength, solidLightSkin_Diffuse_Strength, solidLightSkin_Specular_Strength;
    bool Setting_UseTexture, Setting_Wireframe;
    glm::vec3 lightDirection;
    glm::vec3 solidLightSkin_MaterialColor, solidLightSkin_Ambient, solidLightSkin_Diffuse, solidLightSkin_Specular;

private:
    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboTextureCoordinates, vboTextureDiffuse, vboNormals, vboColors, vboIndices;
    GLuint glUniformMVPMatrix, glUniformMMatrix, glAttributeVertexPosition, glAttributeVertexNormal;
    GLuint glAttributeColor, glFS_CameraPosition;
    GLint glAttributeTextureCoord, glUniformHasTexture, glUniformSamplerTexture;

    std::unique_ptr<ModelFace_LightSource_Directional> solidLight;
};

#endif /* Spaceship_hpp */
