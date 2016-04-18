//
//  CameraModel.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "CameraModel.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

void CameraModel::destroy() {
    delete this->positionX;
    delete this->positionY;
    delete this->positionZ;

    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;

    delete this->rotateCenterX;
    delete this->rotateCenterY;
    delete this->rotateCenterZ;

    delete this->innerLightDirectionX;
    delete this->innerLightDirectionY;
    delete this->innerLightDirectionZ;

    delete this->colorR;
    delete this->colorG;
    delete this->colorB;

    glDisableVertexAttribArray(this->glAttributeVertexPosition);
    glDisableVertexAttribArray(this->glAttributeVertexNormal);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

void CameraModel::init() {
    this->glUtils = new GLUtils();
    this->initProperties();
}

void CameraModel::setModel(objModelFace oFace) {
    this->oFace = oFace;
}

void CameraModel::initProperties() {
    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -6.0f });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -2.0f });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 3.0f });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 300.0f });

    this->rotateCenterX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateCenterY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 35.0f });
    this->rotateCenterZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->innerLightDirectionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->innerLightDirectionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.055f });
    this->innerLightDirectionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.206f });

    this->colorR = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.61f });
    this->colorG = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.61f });
    this->colorB = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.61f });

    this->matrixCamera = glm::mat4(1.0);
    this->matrixModel = glm::mat4(1.0);

    this->showCameraObject = true;
    this->showInWire = false;
}

bool CameraModel::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/camera.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/camera.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glAttributeVertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexNormal");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_color");
        this->glUniformInnerLightDirection = this->glUtils->glGetUniform(this->shaderProgram, "fs_innerLightDirection");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void CameraModel::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->oFace.verticesCount * sizeof(GLfloat), &this->oFace.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->oFace.normalsCount * sizeof(GLfloat), &this->oFace.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexNormal);
    glVertexAttribPointer(this->glAttributeVertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->oFace.indicesCount * sizeof(GLuint), &this->oFace.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void CameraModel::render(glm::mat4 mtxProjection, glm::mat4 mtxCamera, glm::mat4 mtxGrid, bool fixedGridWorld) {
    if (this->glVAO > 0 && this->showCameraObject) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = mtxProjection;
        this->matrixCamera = mtxCamera;

        this->matrixModel = glm::mat4(1.0);
        if (fixedGridWorld)
            this->matrixModel = mtxGrid;
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateCenterX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateCenterY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateCenterZ->point), glm::vec3(0, 0, 1));

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        glUniform3f(this->glUniformColor, this->colorR->point, this->colorG->point, this->colorB->point);
        glUniform3f(this->glUniformInnerLightDirection, this->innerLightDirectionX->point, this->innerLightDirectionY->point, this->innerLightDirectionZ->point);

        // draw
        glBindVertexArray(this->glVAO);
        if (this->showInWire)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, this->oFace.indicesCount, GL_UNSIGNED_INT, nullptr);
        if (this->showInWire)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string CameraModel::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        Settings::Instance()->funcDoLog("Could not read file " + std::string(filePath) + ". File does not exist.");
        return "";
    }
    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}