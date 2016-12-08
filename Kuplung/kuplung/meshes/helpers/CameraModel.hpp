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
    ~CameraModel();
    void init();
    void setModel(const MeshModel meshModel);
    void initProperties();
    bool initShaderProgram();
    void initBuffers();
    void render(const glm::mat4 mtxProjection, const glm::mat4 mtxCamera, const glm::mat4 mtxGrid, const bool fixedGridWorld);
    MeshModel meshModel;

    std::unique_ptr<ObjectCoordinate> positionX, positionY, positionZ;
    std::unique_ptr<ObjectCoordinate> rotateX, rotateY, rotateZ;
    std::unique_ptr<ObjectCoordinate> rotateCenterX, rotateCenterY, rotateCenterZ;
    std::unique_ptr<ObjectCoordinate> innerLightDirectionX, innerLightDirectionY, innerLightDirectionZ;
    std::unique_ptr<ObjectCoordinate> colorR, colorG, colorB;
    bool showCameraObject, showInWire;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;
    glm::mat4 matrixModel;

private:
    std::unique_ptr<GLUtils> glUtils;

    GLuint shaderProgram;
    GLuint shaderVertex, shaderFragment;
    GLuint glVAO;
    GLuint vboVertices, vboNormals, vboIndices;
    GLuint glUniformMVPMatrix, glAttributeVertexPosition, glAttributeVertexNormal, glUniformInnerLightDirection, glUniformColor;
};

#endif /* CameraModel_hpp */
