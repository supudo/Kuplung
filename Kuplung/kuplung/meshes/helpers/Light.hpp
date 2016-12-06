//
//  Light.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/meshes/helpers/RayLine.hpp"

class Light {
public:
    ~Light();
    void init();
    void setModel(MeshModel meshModel);
    void initProperties(LightSourceType type = LightSourceType_Directional);
    bool initShaderProgram();
    void initBuffers(std::string const& assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera);
    glm::vec3 getNewPositionAfterRotation(glm::vec3 rotation);
    bool turnOff_Position;

    MeshModel meshModel;

    std::string title;
    std::string description;
    LightSourceType type;
    bool showLampObject, showLampDirection;

    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> directionX, directionY, directionZ;
    std::unique_ptr<ObjectCoordinate> scaleX, scaleY, scaleZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
    std::unique_ptr<ObjectCoordinate> rotateCenterX, rotateCenterY, rotateCenterZ;
    std::unique_ptr<MaterialColor> ambient, diffuse, specular;
    std::unique_ptr<ObjectCoordinate> lCutOff, lOuterCutOff;
    std::unique_ptr<ObjectCoordinate> lConstant, lLinear, lQuadratic;
    bool showInWire;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    float x, y, z;

    std::unique_ptr<GLUtils> glUtils;
    RayLine* lightDirectionRay;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices;
    GLuint vboTextureDiffuse;
    GLuint glUniformMVPMatrix;
    GLuint glAttributeVertexPosition, glAttributeTextureCoord, glAttributeVertexNormal, glUniformSampler;
    GLuint glUniformUseColor, glUniformColor;
};

#endif /* Light_hpp */
