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
  glUseProgram(0);
  glDeleteProgram(this->shaderProgram);
  glDeleteVertexArrays(1, &this->glVAO_PosX);
  glDeleteVertexArrays(1, &this->glVAO_NegX);
  glDeleteVertexArrays(1, &this->glVAO_PosY);
  glDeleteVertexArrays(1, &this->glVAO_NegY);
  glDeleteVertexArrays(1, &this->glVAO_PosZ);
  glDeleteVertexArrays(1, &this->glVAO_NegZ);
  glDeleteBuffers(1, &this->vboVertices_PosX);
  glDeleteBuffers(1, &this->vboColors_PosX);
  glDeleteBuffers(1, &this->vboIndices_PosX);
  glDeleteBuffers(1, &this->vboVertices_NegX);
  glDeleteBuffers(1, &this->vboColors_NegX);
  glDeleteBuffers(1, &this->vboIndices_NegX);
  glDeleteBuffers(1, &this->vboVertices_PosY);
  glDeleteBuffers(1, &this->vboColors_PosY);
  glDeleteBuffers(1, &this->vboIndices_PosY);
  glDeleteBuffers(1, &this->vboVertices_NegY);
  glDeleteBuffers(1, &this->vboColors_NegY);
  glDeleteBuffers(1, &this->vboIndices_NegY);
  glDeleteBuffers(1, &this->vboVertices_PosZ);
  glDeleteBuffers(1, &this->vboColors_PosZ);
  glDeleteBuffers(1, &this->vboIndices_PosZ);
  glDeleteBuffers(1, &this->vboVertices_NegZ);
  glDeleteBuffers(1, &this->vboColors_NegZ);
  glDeleteBuffers(1, &this->vboIndices_NegZ);
  glDeleteVertexArrays(1, &this->glVAO_PosX);
  glDeleteVertexArrays(1, &this->glVAO_NegX);
  glDeleteVertexArrays(1, &this->glVAO_PosY);
  glDeleteVertexArrays(1, &this->glVAO_NegY);
  glDeleteVertexArrays(1, &this->glVAO_PosZ);
  glDeleteVertexArrays(1, &this->glVAO_NegZ);
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

  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  return success;
}

void AxisLabels::initBuffers() {
  glGenVertexArrays(1, &this->glVAO_PosX);
  glGenVertexArrays(1, &this->glVAO_NegX);
  glGenVertexArrays(1, &this->glVAO_PosY);
  glGenVertexArrays(1, &this->glVAO_NegY);
  glGenVertexArrays(1, &this->glVAO_PosZ);
  glGenVertexArrays(1, &this->glVAO_NegZ);

  std::vector<glm::vec3> dataColors_PosX, dataColors_NegX, dataColors_PosY, dataColors_NegY, dataColors_PosZ, dataColors_NegZ;
  for (int i = 0; i < this->meshModel_PosX.vertices.size(); i++)
    dataColors_PosX.push_back(glm::vec3(this->meshModel_PosX.ModelMaterial.DiffuseColor.r, this->meshModel_PosX.ModelMaterial.DiffuseColor.g, this->meshModel_PosX.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegX.vertices.size(); i++)
    dataColors_NegX.push_back(glm::vec3(this->meshModel_NegX.ModelMaterial.DiffuseColor.r, this->meshModel_NegX.ModelMaterial.DiffuseColor.g, this->meshModel_NegX.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_PosY.vertices.size(); i++)
    dataColors_PosY.push_back(glm::vec3(this->meshModel_PosY.ModelMaterial.DiffuseColor.r, this->meshModel_PosY.ModelMaterial.DiffuseColor.g, this->meshModel_PosY.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegY.vertices.size(); i++)
    dataColors_NegY.push_back(glm::vec3(this->meshModel_NegY.ModelMaterial.DiffuseColor.r, this->meshModel_NegY.ModelMaterial.DiffuseColor.g, this->meshModel_NegY.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_PosZ.vertices.size(); i++)
    dataColors_PosZ.push_back(glm::vec3(this->meshModel_PosZ.ModelMaterial.DiffuseColor.r, this->meshModel_PosZ.ModelMaterial.DiffuseColor.g, this->meshModel_PosZ.ModelMaterial.DiffuseColor.b));
  for (int i = 0; i < this->meshModel_NegZ.vertices.size(); i++)
    dataColors_NegZ.push_back(glm::vec3(this->meshModel_NegZ.ModelMaterial.DiffuseColor.r, this->meshModel_NegZ.ModelMaterial.DiffuseColor.g, this->meshModel_NegZ.ModelMaterial.DiffuseColor.b));

  // X ================
  {
    glBindVertexArray(this->glVAO_PosX);
    // vertices
    glGenBuffers(1, &this->vboVertices_PosX);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_PosX);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosX.countVertices * sizeof(glm::vec3)), &this->meshModel_PosX.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_PosX);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_PosX);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_PosX.size() * sizeof(glm::vec3)), &dataColors_PosX[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_PosX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_PosX);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosX.countIndices) * sizeof(GLuint), &this->meshModel_PosX.indices[0], GL_STATIC_DRAW);
  }

  {
    glBindVertexArray(this->glVAO_NegX);
    // vertices
    glGenBuffers(1, &this->vboVertices_NegX);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_NegX);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegX.countVertices * sizeof(glm::vec3)), &this->meshModel_NegX.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_NegX);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_NegX);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_NegX.size() * sizeof(glm::vec3)), &dataColors_NegX[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_NegX);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_NegX);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegX.countIndices) * sizeof(GLuint), &this->meshModel_NegX.indices[0], GL_STATIC_DRAW);
  }

  // Y ================
  {
    glBindVertexArray(this->glVAO_PosY);
    // vertices
    glGenBuffers(1, &this->vboVertices_PosY);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_PosY);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosY.countVertices * sizeof(glm::vec3)), &this->meshModel_PosY.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_PosY);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_PosY);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_PosY.size() * sizeof(glm::vec3)), &dataColors_PosY[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_PosY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_PosY);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosY.countIndices) * sizeof(GLuint), &this->meshModel_PosY.indices[0], GL_STATIC_DRAW);
  }

  {
    glBindVertexArray(this->glVAO_NegY);
    // vertices
    glGenBuffers(1, &this->vboVertices_NegY);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_NegY);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegY.countVertices * sizeof(glm::vec3)), &this->meshModel_NegY.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_NegY);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_NegY);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_NegY.size() * sizeof(glm::vec3)), &dataColors_NegY[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_NegY);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_NegY);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegY.countIndices) * sizeof(GLuint), &this->meshModel_NegY.indices[0], GL_STATIC_DRAW);
  }

  // X ================
  {
    glBindVertexArray(this->glVAO_PosZ);
    // vertices
    glGenBuffers(1, &this->vboVertices_PosZ);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_PosZ);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosZ.countVertices * sizeof(glm::vec3)), &this->meshModel_PosZ.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_PosZ);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_PosZ);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_PosZ.size() * sizeof(glm::vec3)), &dataColors_PosZ[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_PosZ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_PosZ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_PosZ.countIndices) * sizeof(GLuint), &this->meshModel_PosZ.indices[0], GL_STATIC_DRAW);
  }

  {
    glBindVertexArray(this->glVAO_NegZ);
    // vertices
    glGenBuffers(1, &this->vboVertices_NegZ);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices_NegZ);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegZ.countVertices * sizeof(glm::vec3)), &this->meshModel_NegZ.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // colors
    glGenBuffers(1, &this->vboColors_NegZ);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors_NegZ);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(dataColors_NegZ.size() * sizeof(glm::vec3)), &dataColors_NegZ[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // indices
    glGenBuffers(1, &this->vboIndices_NegZ);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices_NegZ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel_NegZ.countIndices) * sizeof(GLuint), &this->meshModel_NegZ.indices[0], GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
}

void AxisLabels::render(const glm::mat4& mtxProjection, const glm::mat4& mtxCamera, int position) {
  if (position != this->ahPosition)
    this->initBuffers();

  glUseProgram(this->shaderProgram);

  glBindVertexArray(this->glVAO_PosX);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(position, 0, 0))));
  glDrawElements(GL_TRIANGLES, this->meshModel_PosX.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(this->glVAO_NegX);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(-position - 1, 0, 0))));
  glDrawElements(GL_TRIANGLES, this->meshModel_NegX.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(this->glVAO_PosY);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(0, position, 0))));
  glDrawElements(GL_TRIANGLES, this->meshModel_PosY.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(this->glVAO_NegY);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(0, -position, 0))));
  glDrawElements(GL_TRIANGLES, this->meshModel_NegY.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(this->glVAO_PosZ);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, position))));
  glDrawElements(GL_TRIANGLES, this->meshModel_PosZ.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(this->glVAO_NegZ);
  glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxProjection * mtxCamera * glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -position))));
  glDrawElements(GL_TRIANGLES, this->meshModel_NegZ.countIndices, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}
