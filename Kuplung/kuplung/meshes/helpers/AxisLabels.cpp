//
//  AxisLabels.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "AxisLabels.hpp"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>

AxisLabels::~AxisLabels() {
  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteProgram(this->shaderProgram);

  glDisableVertexAttribArray(0);

  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  glDeleteVertexArrays(1, &this->glVAO);
}

AxisLabels::AxisLabels() {
}

void AxisLabels::setModels(const MeshModel& meshModel_PosX, const MeshModel& meshModel_NegX, const MeshModel& meshModel_PosY,
  const MeshModel& meshModel_NegY, const MeshModel& meshModel_PosZ, const MeshModel& meshModel_NegZ,
  int position) {
  this->meshModel_PosX = meshModel_PosX;
  this->meshModel_NegX = meshModel_NegX;
  this->meshModel_PosY = meshModel_PosY;
  this->meshModel_NegY = meshModel_NegY;
  this->meshModel_PosZ = meshModel_PosZ;
  this->meshModel_NegZ = meshModel_NegZ;
  this->ahPosition = position;
}

void AxisLabels::initProperties() {}

const bool AxisLabels::initShaderProgram() {
  bool success = true;

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/axis_labels.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/axis_labels.frag";
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
    Settings::Instance()->funcDoLog("[AxisLabels] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");

  return success;
}

void AxisLabels::initBuffers() {
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  auto fixPosition = [&pos = this->ahPosition](glm::vec3 vertex) {
    return vertex * glm::vec3(pos, 0, 0);
  };

  std::for_each(std::begin(this->meshModel_PosX.vertices), std::end(this->meshModel_PosX.vertices), fixPosition);

  // TODO(supudo): convert directly the ahPosition into the vertices
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_PosX.vertices.begin(), this->meshModel_PosX.vertices.end());
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_NegX.vertices.begin(), this->meshModel_NegX.vertices.end());
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_PosY.vertices.begin(), this->meshModel_PosY.vertices.end());
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_NegY.vertices.begin(), this->meshModel_NegY.vertices.end());
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_PosZ.vertices.begin(), this->meshModel_PosZ.vertices.end());
  this->dataVertices.insert(this->dataVertices.end(), this->meshModel_NegZ.vertices.begin(), this->meshModel_NegZ.vertices.end());

  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_PosX.indices.begin(), this->meshModel_PosX.indices.end());
  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_NegX.indices.begin(), this->meshModel_NegX.indices.end());
  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_PosY.indices.begin(), this->meshModel_PosY.indices.end());
  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_NegY.indices.begin(), this->meshModel_NegY.indices.end());
  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_PosZ.indices.begin(), this->meshModel_PosZ.indices.end());
  this->dataIndices.insert(this->dataIndices.end(), this->meshModel_NegZ.indices.begin(), this->meshModel_NegZ.indices.end());

  for (int i = 0; i < this->meshModel_PosX.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_PosX.ModelMaterial.DiffuseColor.r, this->meshModel_PosX.ModelMaterial.DiffuseColor.g, this->meshModel_PosX.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegX.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_NegX.ModelMaterial.DiffuseColor.r, this->meshModel_NegX.ModelMaterial.DiffuseColor.g, this->meshModel_NegX.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_PosY.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_PosY.ModelMaterial.DiffuseColor.r, this->meshModel_PosY.ModelMaterial.DiffuseColor.g, this->meshModel_PosY.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegY.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_NegY.ModelMaterial.DiffuseColor.r, this->meshModel_NegY.ModelMaterial.DiffuseColor.g, this->meshModel_NegY.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_PosZ.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_PosZ.ModelMaterial.DiffuseColor.r, this->meshModel_PosZ.ModelMaterial.DiffuseColor.g, this->meshModel_PosZ.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegZ.vertices.size(); i++)
    this->dataColors.push_back(glm::vec3(this->meshModel_NegZ.ModelMaterial.DiffuseColor.r, this->meshModel_NegZ.ModelMaterial.DiffuseColor.g, this->meshModel_NegZ.ModelMaterial.DiffuseColor.b));

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->dataIndices.size()) * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);
}

void AxisLabels::render(const glm::mat4& mtxProjection, const glm::mat4& mtxCamera, int position) {
  if (position != this->ahPosition)
    this->initBuffers();

  if (this->glVAO > 0) {
    glUseProgram(this->shaderProgram);

    glm::mat4 mvpMatrix = mtxProjection * mtxCamera * glm::mat4(1.0);
    glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glBindVertexArray(this->glVAO);
    glDrawElements(GL_TRIANGLES, this->dataIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glUseProgram(0);
  }
}
