//
//  Light.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/parsers/ModelObject.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLUtils.hpp"

class Light {
public:
    ~Light();
    void destroy();
    void init(std::function<void(std::string)> doLog);
    void setModel(objModelFace oFace);
    void initProperties();
    bool initShaderProgram();
    void initBuffers(std::string assetsFolder);
    void render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 mtxGrid, bool fixedGridWorld);
    objModelFace oFace;

    std::string title;
    std::string description;
    LightSourceType type;
    bool showLampObject, showLampDirection;

    ObjectEye *eyeSettings;
    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *directionX, *directionY, *directionZ;
    ObjectCoordinate *scaleX, *scaleY, *scaleZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;
    MaterialColor *ambient, *diffuse, *specular;
    ObjectCoordinate *lCutOff, *lOuterCutOff;
    ObjectCoordinate *lConstant, *lLiteral, *lQuadratic;
    bool showInWire;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::function<void(std::string)> doLogFunc;
    float x, y, z;

    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboTextureCoordinates, vboIndices;
    GLuint vboTextureDiffuse;
    GLuint glUniformMVPMatrix;
    GLuint glAttributeVertexPosition, glAttributeTextureCoord, glAttributeVertexNormal, glUniformSampler;
    GLuint glUniformUseColor, glUniformColor;

    std::string readFile(const char *filePath);
    void doLog(std::string logMessage);
};

#endif /* Light_hpp */
