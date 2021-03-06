//
//  RayLine.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RayLine.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

RayLine::~RayLine() {
  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboIndices);
  glDeleteBuffers(1, &this->vboColors);
  glDisableVertexAttribArray(0);
  glDeleteProgram(this->shaderProgram);
  glDeleteVertexArrays(1, &this->glVAO);
}

void RayLine::init() {
  this->initProperties();
}

void RayLine::initProperties() {
  this->matrixModel = glm::mat4(1.0);
}

const bool RayLine::initShaderProgram() {
  bool success = true;

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/ray_line.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/ray_line.frag";
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
    Settings::Instance()->funcDoLog("[RayLine] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else {
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
    this->glUniformColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_color");
  }

  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  return success;
}

void RayLine::initBuffers(const glm::vec3& vecFrom, const glm::vec3& vecTo) {
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  this->dataVertices = {vecFrom.x, vecFrom.y, vecFrom.z, vecTo.x, vecTo.y, vecTo.z};

  this->dataColors = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  this->dataIndices = {0, 1, 2, 3, 4, 5};

  // vertices
  glGenBuffers(1, &this->vboVertices);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataVertices.size()) * sizeof(GLfloat), &this->dataVertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // colors
  glGenBuffers(1, &this->vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->dataColors.size() * sizeof(GLfloat)), &this->dataColors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // indices
  glGenBuffers(1, &this->vboIndices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size()) * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void RayLine::render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera) {
  if (this->glVAO > 0) {
    glUseProgram(this->shaderProgram);

    glm::mat4 mvpMatrix = matrixProjection * matrixCamera * this->matrixModel;
    glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    // draw
    glBindVertexArray(this->glVAO);

    glLineWidth(2.5f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    glDrawArrays(GL_LINE_STRIP, 0, 2);

    glBindVertexArray(0);

    glUseProgram(0);
  }
}
