//
//  WorldGrid.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "WorldGrid.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

WorldGrid::~WorldGrid() {
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

  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteProgram(this->shaderProgram);

  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  glDeleteVertexArrays(1, &this->glVAO);
}

WorldGrid::WorldGrid() {
  this->gridSize = 10;
  this->gridSizeVertex = 10;
  this->zIndex = 0;
  this->initProperties();
}

void WorldGrid::initProperties() {
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

const bool WorldGrid::initShaderProgram() {
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
    Settings::Instance()->funcDoLog("[WorldGrid] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else {
    this->glAttributeActAsMirror = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "a_actAsMirror");
    this->glAttributeAlpha = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "a_alpha");
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
  }

  return success;
}

void WorldGrid::initBuffers(const int& gridSize, const float& unitSize) {
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  if (!this->actAsMirror) {
    this->actAsMirrorNeedsChange = true;
    this->gridSize = gridSize;
    this->gridSizeVertex = this->gridSize;

    if (this->gridSizeVertex % 2 == 0)
      this->gridSizeVertex += 1;

    this->dataVertices.clear();
    this->dataTexCoords.clear();
    this->dataNormals.clear();
    this->dataColors.clear();
    this->dataIndices.clear();

    const int gridMinus = this->gridSizeVertex / 2;
    std::vector<GridMeshPoint3D> verticesData;
    int indc = 0;
    for (int i = 0; i < (this->gridSizeVertex * 2); i++) {
      for (int j = 0; j < this->gridSizeVertex; j++) {
        this->dataIndices.push_back(indc);
        indc++;
        if (i >= this->gridSizeVertex) {
          GridMeshPoint3D p;
          p.x = (j - gridMinus) * unitSize;
          p.y = 0;
          p.z = (i - gridMinus) * unitSize;
          verticesData.push_back(p);
          if (p.z < 0 || p.z > 0) {
            this->dataColors.push_back(0.7f);
            this->dataColors.push_back(0.7f);
            this->dataColors.push_back(0.7f);
          }
          else {
            this->dataColors.push_back(1.0f);
            this->dataColors.push_back(0.0f);
            this->dataColors.push_back(0.0f);
          }
        }
        else {
          GridMeshPoint3D p;
          p.x = (i - this->gridSizeVertex - gridMinus) * unitSize;
          p.y = 0;
          p.z = (j - gridMinus) * unitSize;
          verticesData.push_back(p);
          if (p.x < 0 || p.x > 0) {
            this->dataColors.push_back(0.7f);
            this->dataColors.push_back(0.7f);
            this->dataColors.push_back(0.7f);
          }
          else {
            this->dataColors.push_back(0.0f);
            this->dataColors.push_back(0.0f);
            this->dataColors.push_back(1.0f);
          }
        }
      }
    }

    this->zIndex = (int)verticesData.size();

    GridMeshPoint3D p_z_Minus_Down;
    p_z_Minus_Down.x = 0.0f;
    p_z_Minus_Down.y = static_cast<float>(-1.0f * gridMinus);
    p_z_Minus_Down.z = 0.0f;
    verticesData.push_back(p_z_Minus_Down);
    this->dataColors.push_back(0.0f);
    this->dataColors.push_back(1.0f);
    this->dataColors.push_back(0.0f);

    GridMeshPoint3D p_z_Plus_Up;
    p_z_Plus_Up.x = 0.0f;
    p_z_Plus_Up.y = static_cast<float>(gridMinus);
    p_z_Plus_Up.z = 0.0f;
    verticesData.push_back(p_z_Plus_Up);
    this->dataColors.push_back(0.0f);
    this->dataColors.push_back(1.0f);
    this->dataColors.push_back(0.0f);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(verticesData.size() * sizeof(GridMeshPoint3D)), &verticesData[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataColors.size() * sizeof(GLfloat)), &this->dataColors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    // TODO: fix for windows!
#ifndef _WIN32
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices[0], GL_STATIC_DRAW);
#endif
  }
  else {
    this->actAsMirrorNeedsChange = false;
    float planePoint = static_cast<float>(this->gridSize / 2);
    this->dataVertices = {planePoint, 0.0, planePoint, planePoint, 0.0, -1 * planePoint, -1 * planePoint, 0.0, -1 * planePoint, -1 * planePoint, 0.0, planePoint, planePoint, 0.0, planePoint, -1 * planePoint, 0.0, -1 * planePoint};
    this->dataTexCoords = {0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    this->dataNormals = {
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
    };
    this->dataColors = {
      0.7f, 0.7f, 0.7f,
      0.7f, 0.7f, 0.7f,
      0.7f, 0.7f, 0.7f,
      0.7f, 0.7f, 0.7f,
    };

    this->dataIndices = {0, 1, 2, 3, 4, 5};

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataVertices.size() * sizeof(GLfloat)), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataColors.size() * sizeof(GLfloat)), &this->dataColors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices[0], GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}

void WorldGrid::render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const bool& showZAxis) {
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

      // TODO: minimize draw calls - make it 1!
      for (int i = 0; i < this->gridSizeVertex * 2; i++)
        glDrawArrays(GL_LINE_STRIP, this->gridSizeVertex * i, this->gridSizeVertex);
      for (int i = 0; i < this->gridSizeVertex; i++)
        glDrawArrays(GL_LINE_STRIP, 0, this->gridSizeVertex);
      if (showZAxis)
        glDrawArrays(GL_LINES, this->zIndex, 2);
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
