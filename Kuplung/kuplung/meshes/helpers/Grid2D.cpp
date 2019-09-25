//
//  Grid2D.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Grid2D.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

Grid2D::~Grid2D() {
  this->positionX.reset();
  this->positionY.reset();
  this->positionZ.reset();

  this->scaleX.reset();
  this->scaleY.reset();
  this->scaleZ.reset();

  this->rotateX.reset();
  this->rotateY.reset();
  this->rotateZ.reset();

  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboIndices);
  glDeleteBuffers(1, &this->vboColors);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDeleteProgram(this->shaderProgram);
  glDeleteVertexArrays(1, &this->glVAO);
}

Grid2D::Grid2D() {
  this->gridSize = 10;
  this->gridSizeVertex = 10;
  this->zIndex = 0;
  this->initProperties();
}

void Grid2D::initProperties() {
  this->showGrid = true;
  this->actAsMirror = false;
  this->actAsMirrorNeedsChange = true;
  this->transparency = 0.5;

  this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->positionZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

  this->scaleX = std::make_unique<ObjectCoordinate>(false, 1.0f);
  this->scaleY = std::make_unique<ObjectCoordinate>(false, 1.0f);
  this->scaleZ = std::make_unique<ObjectCoordinate>(false, 1.0f);

  this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f);
  this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

  this->matrixModel = glm::mat4(1.0);

  this->mirrorSurface = std::make_unique<MirrorSurface>();

  this->mirrorSurface->translateX = 0.0;
  this->mirrorSurface->translateY = 5.0;
  this->mirrorSurface->translateZ = -5.0;
  this->mirrorSurface->rotateX = 90.0f;
  this->mirrorSurface->rotateY = 0.0f;
  this->mirrorSurface->rotateZ = 0.0f;
}

const bool Grid2D::initShaderProgram() {
  bool success = true;

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/grid.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/grid.frag";
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
    Settings::Instance()->funcDoLog("[Grid2D] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else {
    this->glAttributeActAsMirror = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "a_actAsMirror");
    this->glAttributeAlpha = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "a_alpha");
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
  }
  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  return success;
}

void Grid2D::initBuffers(const int& gridSize, const float& unitSize) {
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  if (!this->actAsMirror) {
    this->actAsMirrorNeedsChange = true;
    this->gridSize = gridSize % 2 == 0 ? gridSize + 1 : gridSize;
    this->gridSizeVertex = this->gridSize;

    if (this->gridSizeVertex % 2 == 0)
      this->gridSizeVertex += 1;

    this->dataVertices.clear();
    this->dataTexCoords.clear();
    this->dataNormals.clear();
    this->dataColors.clear();
    this->dataIndices.clear();

    GLuint indiceCounter = 0;
    const int perLines = std::ceil(this->gridSizeVertex / 2);

    // +
    for (float z = perLines; z > 0; z--) {
      this->dataVertices.push_back(glm::vec3(-perLines * unitSize, 0.0, -z * unitSize));
      this->dataVertices.push_back(glm::vec3(perLines * unitSize, 0.0, -z * unitSize));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataIndices.push_back(indiceCounter++);
      this->dataIndices.push_back(indiceCounter++);
    }

    // X
    this->dataVertices.push_back(glm::vec3(-perLines * unitSize, 0.0, 0.0));
    this->dataVertices.push_back(glm::vec3(perLines * unitSize, 0.0, 0.0));
    this->dataColors.push_back(glm::vec3(1.0, 0.0, 0.0));
    this->dataColors.push_back(glm::vec3(1.0, 0.0, 0.0));
    this->dataIndices.push_back(indiceCounter++);
    this->dataIndices.push_back(indiceCounter++);
    // X

    for (float z = 1.0; z <= perLines; z++) {
      this->dataVertices.push_back(glm::vec3(-perLines * unitSize, 0.0, z * unitSize));
      this->dataVertices.push_back(glm::vec3(perLines * unitSize, 0.0, z * unitSize));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataIndices.push_back(indiceCounter++);
      this->dataIndices.push_back(indiceCounter++);
    }

    // -
    for (float x = perLines; x > 0; x--) {
      this->dataVertices.push_back(glm::vec3(-x * unitSize, 0.0, -perLines * unitSize));
      this->dataVertices.push_back(glm::vec3(-x * unitSize, 0.0, perLines * unitSize));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataIndices.push_back(indiceCounter++);
      this->dataIndices.push_back(indiceCounter++);
    }

    // Z
    this->dataVertices.push_back(glm::vec3(0.0, 0.0, perLines * unitSize));
    this->dataVertices.push_back(glm::vec3(0.0, 0.0, -perLines * unitSize));
    this->dataColors.push_back(glm::vec3(0.0, 0.0, 1.0));
    this->dataColors.push_back(glm::vec3(0.0, 0.0, 1.0));
    this->dataIndices.push_back(indiceCounter++);
    this->dataIndices.push_back(indiceCounter++);
    // Z

    for (float x = 1.0; x <= perLines; x++) {
      this->dataVertices.push_back(glm::vec3(x * unitSize, 0.0, -perLines * unitSize));
      this->dataVertices.push_back(glm::vec3(x * unitSize, 0.0, perLines * unitSize));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataColors.push_back(glm::vec3(0.7, 0.7, 0.7));
      this->dataIndices.push_back(indiceCounter++);
      this->dataIndices.push_back(indiceCounter++);
    }

    this->zIndex = (int)this->dataVertices.size();

    // Y
    this->dataVertices.push_back(glm::vec3(0.0, perLines * unitSize, 0.0));
    this->dataVertices.push_back(glm::vec3(0.0, -perLines * unitSize, 0.0));
    this->dataColors.push_back(glm::vec3(0.0, 1.0, 0.0));
    this->dataColors.push_back(glm::vec3(0.0, 1.0, 0.0));
    this->dataIndices.push_back(indiceCounter++);
    this->dataIndices.push_back(indiceCounter++);

    this->dataVertices.push_back(glm::vec3(0.0, perLines * unitSize, 0.0));
    this->dataVertices.push_back(glm::vec3(0.0, -perLines * unitSize, 0.0));
    this->dataColors.push_back(glm::vec3(0.0, 1.0, 0.0));
    this->dataColors.push_back(glm::vec3(0.0, 1.0, 0.0));
    this->dataIndices.push_back(indiceCounter++);
    this->dataIndices.push_back(indiceCounter++);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->dataVertices.size() * sizeof(glm::vec3)), &this->dataVertices.at(0), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->dataColors.size() * sizeof(glm::vec3)), &this->dataColors.at(0), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices.at(0), GL_STATIC_DRAW);
  }
  else {
    this->actAsMirrorNeedsChange = false;
    float planePoint = static_cast<float>(this->gridSize / 2);
    this->dataVertices = {
      glm::vec3(planePoint, 0.0, planePoint),
      glm::vec3(planePoint, 0.0, -1 * planePoint),
      glm::vec3(-1 * planePoint, 0.0, -1 * planePoint),
      glm::vec3(-1 * planePoint, 0.0, planePoint),
      glm::vec3(planePoint, 0.0, planePoint),
      glm::vec3(-1 * planePoint, 0.0, -1 * planePoint)
    };
    this->dataTexCoords = { 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f };
    this->dataNormals = {
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
    };
    this->dataColors = {
      glm::vec3(0.7f, 0.7f, 0.7f),
      glm::vec3(0.7f, 0.7f, 0.7f),
      glm::vec3(0.7f, 0.7f, 0.7f),
      glm::vec3(0.7f, 0.7f, 0.7f),
    };

    this->dataIndices = { 0, 1, 2, 3, 4, 5 };

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataVertices.size() * sizeof(glm::vec3)), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataColors.size() * sizeof(glm::vec3)), &this->dataColors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices[0], GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}

void Grid2D::render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const bool& showZAxis) {
  if (this->glVAO > 0 && this->showGrid) {
    glUseProgram(this->shaderProgram);

    this->matrixModel = glm::mat4(1.0);
    this->matrixModel = glm::scale(this->matrixModel, glm::vec3(this->scaleX->point, this->scaleY->point, this->scaleZ->point));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

    glm::mat4 mvpMatrix = matrixProjection * matrixCamera * this->matrixModel;
    glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // draw
    glBindVertexArray(this->glVAO);

    if (this->actAsMirror && this->actAsMirrorNeedsChange)
      this->initBuffers(this->gridSize, 1.0);
    else if (!this->actAsMirror && !this->actAsMirrorNeedsChange)
      this->initBuffers(this->gridSize, 1.0);

    if (!this->actAsMirror) {
      glLineWidth(1.0f);

      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glDisable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glUniform1f(this->glAttributeAlpha, 1.0);
      glUniform1i(this->glAttributeActAsMirror, 0);

      glDrawElements(GL_LINES, this->dataIndices.size(), GL_UNSIGNED_INT, nullptr);
    }
    else {
      glEnable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glEnable(GL_BLEND);

      glUniform1f(this->glAttributeAlpha, this->transparency);
      glUniform1i(this->glAttributeActAsMirror, 1);

      glDepthMask(GL_FALSE);

      glDrawElements(GL_TRIANGLES, sizeof(this->dataIndices), GL_UNSIGNED_INT, nullptr);

      glDepthMask(GL_TRUE);

      glDisable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
    }

    glBindVertexArray(0);

    glUseProgram(0);
  }
}
