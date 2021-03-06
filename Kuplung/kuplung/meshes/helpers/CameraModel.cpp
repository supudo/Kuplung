//
//  CameraModel.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "CameraModel.hpp"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>

CameraModel::~CameraModel() {
  this->positionX.reset();
  this->positionY.reset();
  this->positionZ.reset();

  this->rotateX.reset();
  this->rotateY.reset();
  this->rotateZ.reset();

  this->rotateCenterX.reset();
  this->rotateCenterY.reset();
  this->rotateCenterZ.reset();

  this->innerLightDirectionX.reset();
  this->innerLightDirectionY.reset();
  this->innerLightDirectionZ.reset();

  this->colorR.reset();
  this->colorG.reset();
  this->colorB.reset();

  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboNormals);
  glDeleteBuffers(1, &this->vboIndices);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDeleteProgram(this->shaderProgram);
  glDeleteVertexArrays(1, &this->glVAO);
}

CameraModel::CameraModel() : meshModel() {
  this->initProperties();
  this->showCameraObject = true;
  this->showInWire = false;
}

void CameraModel::setModel(MeshModel const& meshModel) {
  this->meshModel = meshModel;
}

void CameraModel::initProperties() {
  this->positionX = std::make_unique<ObjectCoordinate>(false, -6.0f);
  this->positionY = std::make_unique<ObjectCoordinate>(false, -2.0f);
  this->positionZ = std::make_unique<ObjectCoordinate>(false, 3.0f);

  this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateZ = std::make_unique<ObjectCoordinate>(false, 300.0f);

  this->rotateCenterX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateCenterY = std::make_unique<ObjectCoordinate>(false, 35.0f);
  this->rotateCenterZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

  this->innerLightDirectionX = std::make_unique<ObjectCoordinate>(false, 1.0f);
  this->innerLightDirectionY = std::make_unique<ObjectCoordinate>(false, 0.055f);
  this->innerLightDirectionZ = std::make_unique<ObjectCoordinate>(false, 0.206f);

  this->colorR = std::make_unique<ObjectCoordinate>(false, 0.61f);
  this->colorG = std::make_unique<ObjectCoordinate>(false, 0.61f);
  this->colorB = std::make_unique<ObjectCoordinate>(false, 0.61f);

  this->matrixModel = glm::mat4(1.0);

  this->showCameraObject = true;
  this->showInWire = false;
}

const bool CameraModel::initShaderProgram() {
  bool success = true;

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/camera.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/camera.frag";
  std::string shaderSourceFragment = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_fragment = shaderSourceFragment.c_str();

  this->shaderProgram = glCreateProgram();

  bool shaderCompilation = true;
  shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
  shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

  if (!shaderCompilation)
    return false;

  glLinkProgram(this->shaderProgram);

  GLint programSuccess = GL_TRUE;
  glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE) {
    Settings::Instance()->funcDoLog("[CameraModel] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else {
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
    this->glUniformColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_color");
    this->glUniformInnerLightDirection = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_innerLightDirection");
  }

  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

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
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel.vertices.size() * sizeof(glm::vec3)), &this->meshModel.vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // normals
  glGenBuffers(1, &this->vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel.normals.size() * sizeof(glm::vec3)), &this->meshModel.normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // indices
  glGenBuffers(1, &this->vboIndices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel.countIndices) * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void CameraModel::render(const glm::mat4& mtxProjection, const glm::mat4& mtxCamera, const glm::mat4& mtxGrid, const bool& fixedGridWorld) {
  if (this->glVAO > 0 && this->showCameraObject) {
    glUseProgram(this->shaderProgram);

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

    glm::mat4 mvpMatrix = mtxProjection * mtxCamera * this->matrixModel;
    glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glUniform3f(this->glUniformColor, this->colorR->point, this->colorG->point, this->colorB->point);
    glUniform3f(this->glUniformInnerLightDirection, this->innerLightDirectionX->point, this->innerLightDirectionY->point, this->innerLightDirectionZ->point);

    // draw
    glBindVertexArray(this->glVAO);
    if (this->showInWire)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
    if (this->showInWire)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);

    glUseProgram(0);
  }
}
