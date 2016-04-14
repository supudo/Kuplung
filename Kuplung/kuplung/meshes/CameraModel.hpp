//
//  CameraModel.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef CameraModel_hpp
#define CameraModel_hpp

#include <glm/glm.hpp>
#include <string>
#include "kuplung/settings/Settings.h"
#include "kuplung/objects/ObjectDefinitions.h"
#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/utilities/gl/GLUtils.hpp"
#include "kuplung/utilities/parsers/ModelObject.h"

class CameraModel {
public:
    void destroy();
    void init();
    void setModel(objModelFace oFace);
    void initProperties();
    bool initShaderProgram();
    void initBuffers();
    void render(glm::mat4 mtxProjection, glm::mat4 mtxCamera, glm::mat4 mtxGrid, bool fixedGridWorld);
    objModelFace oFace;

    ObjectCoordinate *positionX, *positionY, *positionZ;
    ObjectCoordinate *rotateX, *rotateY, *rotateZ;
    ObjectCoordinate *rotateCenterX, *rotateCenterY, *rotateCenterZ;
    ObjectCoordinate *innerLightDirectionX, *innerLightDirectionY, *innerLightDirectionZ;
    ObjectCoordinate *colorR, *colorG, *colorB;
    bool showCameraObject, showInWire;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    GLUtils *glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glUniformInnerLightDirection, glUniformColor;

    std::string readFile(const char *filePath);
};

#endif /* CameraModel_hpp */
