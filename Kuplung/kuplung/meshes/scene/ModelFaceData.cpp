//
//  ModelFaceData.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceData.hpp"
#include "kuplung/utilities/imgui/imguizmo/ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "kuplung/utilities/imgui/imgui.h"

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

ModelFaceData::~ModelFaceData() {
  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboNormals);
  glDeleteBuffers(1, &this->vboTextureCoordinates);
  glDeleteBuffers(1, &this->vboIndices);
  glDeleteBuffers(1, &this->vboTangents);
  glDeleteBuffers(1, &this->vboBitangents);

  glDeleteTextures(1, &this->vboTextureAmbient);
  glDeleteTextures(1, &this->vboTextureDiffuse);
  glDeleteTextures(1, &this->vboTextureSpecular);
  glDeleteTextures(1, &this->vboTextureSpecularExp);
  glDeleteTextures(1, &this->vboTextureDissolve);
  glDeleteTextures(1, &this->vboTextureBump);
  glDeleteTextures(1, &this->vboTextureDisplacement);

  GLint maxColorAttachments = 1;
  glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
  GLenum att = GL_COLOR_ATTACHMENT0;
  for (GLuint colorAttachment = 0; colorAttachment < static_cast<GLuint>(maxColorAttachments); colorAttachment++) {
    att += colorAttachment;
    GLint param;
    GLuint objName = -1;
    glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
    if (GL_RENDERBUFFER == param) {
      glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
      objName = reinterpret_cast<GLuint*>(&param)[0];
      glDeleteRenderbuffers(1, &objName);
    }
    else if (GL_TEXTURE == param) {
      glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
      objName = reinterpret_cast<GLuint*>(&param)[0];
      glDeleteTextures(1, &objName);
    }
  }

  glDeleteVertexArrays(1, &this->glVAO);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}

void ModelFaceData::init(MeshModel const& model, std::string const& assetsFolder) {
  ModelFaceBase::init(model, assetsFolder);

  this->meshModel = model;
  this->assetsFolder = assetsFolder;

  this->initBuffers();
}

void ModelFaceData::initBuffers() {
  ModelFaceBase::initBuffers();
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  // vertices
  glGenBuffers(1, &this->vboVertices);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->meshModel.vertices.size() * sizeof(glm::vec3)), &this->meshModel.vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // normals
  glGenBuffers(1, &this->vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->meshModel.normals.size() * sizeof(glm::vec3)), &this->meshModel.normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

  // textures and colors
  if (this->meshModel.texture_coordinates.size() > 0) {
    glGenBuffers(1, &this->vboTextureCoordinates);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->meshModel.texture_coordinates.size() * sizeof(glm::vec2)), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    // ambient texture image
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureAmbient, objMaterialImageType_Ambient, &this->vboTextureAmbient);

    // diffuse texture image
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Diffuse, &this->vboTextureDiffuse);

    // specular texture image
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecular, objMaterialImageType_Specular, &this->vboTextureSpecular);

    // specular-exp texture image
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecularExp, objMaterialImageType_SpecularExp, &this->vboTextureSpecularExp);

    // dissolve texture image
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDissolve, objMaterialImageType_Dissolve, &this->vboTextureDissolve);

    // bump map texture
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureBump, objMaterialImageType_Bump, &this->vboTextureBump);

    // displacement map texture
    ModelFaceBase::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDisplacement, objMaterialImageType_Displacement, &this->vboTextureDisplacement);
  }

  // indices
  glGenBuffers(1, &this->vboIndices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->meshModel.countIndices) * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

  if (!this->meshModel.ModelMaterial.TextureBump.Image.empty() && this->meshModel.vertices.size() > 0 && this->meshModel.texture_coordinates.size() > 0 && this->meshModel.normals.size() > 0) {
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    this->mathHelper->computeTangentBasis(this->meshModel.vertices, this->meshModel.texture_coordinates, this->meshModel.normals, tangents, bitangents);

    // tangents
    glGenBuffers(1, &this->vboTangents);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTangents);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(tangents.size() * sizeof(glm::vec3)), &tangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    // bitangents
    glGenBuffers(1, &this->vboBitangents);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboBitangents);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bitangents.size() * sizeof(glm::vec3)), &bitangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
  }

  glGenQueries(1, &this->occQuery);

  glBindVertexArray(0);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}

void ModelFaceData::renderModel(const bool useTessellation) {
  if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  if (Settings::Instance()->grOcclusionCulling)
    glBeginConditionalRender(this->occQuery, GL_QUERY_BY_REGION_WAIT);

  glBindVertexArray(this->glVAO);

  if (useTessellation)
    glDrawElements(GL_PATCHES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
  else
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, 0);

  if (Settings::Instance()->grOcclusionCulling)
    glEndConditionalRender();

  glBindVertexArray(0);

  if (this->Setting_Wireframe || Settings::Instance()->wireframesMode || this->Setting_ModelViewSkin == ViewModelSkin_Wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glm::mat4 matrixBB = glm::mat4(1.0f);
  matrixBB *= this->matrixProjection;
  matrixBB *= this->matrixCamera;
  matrixBB *= this->matrixModel;

  if (Settings::Instance()->ShowBoundingBox && this->so_selectedYn)
    this->boundingBox->render(matrixBB, this->so_outlineColor);

  if (this->vertexSphereVisible) {
    this->vertexSphere->isSphere = this->vertexSphereIsSphere;
    this->vertexSphere->showWireframes = this->vertexSphereShowWireframes;
    this->vertexSphere->initBuffers(this->meshModel, this->vertexSphereSegments, this->vertexSphereRadius);
    this->vertexSphere->render(matrixBB, vertexSphereColor);
  }

  if (this->getOptionsSelected() && (this->Setting_Gizmo_Rotate || this->Setting_Gizmo_Translate || this->Setting_Gizmo_Scale)) {
    ImGuizmo::Enable(true);
    ImGuizmo::OPERATION gizmo_operation = ImGuizmo::TRANSLATE;
    if (this->Setting_Gizmo_Rotate)
      gizmo_operation = ImGuizmo::ROTATE;
    else if (this->Setting_Gizmo_Scale)
      gizmo_operation = ImGuizmo::SCALE;
    glm::mat4 mtx = glm::mat4(1.0);
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(glm::value_ptr(this->matrixCamera), glm::value_ptr(this->matrixProjection), gizmo_operation, ImGuizmo::LOCAL, glm::value_ptr(this->matrixModel), glm::value_ptr(mtx));

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(mtx, scale, rotation, translation, skew, perspective);

    if (this->Setting_Gizmo_Translate) {
      this->positionX->point += translation.x;
      this->positionY->point += translation.y;
      this->positionZ->point += translation.z;
    }

    if (this->Setting_Gizmo_Rotate) {
      this->rotateX->point += glm::degrees(rotation.x);
      this->rotateY->point += glm::degrees(rotation.y);
      this->rotateZ->point += glm::degrees(rotation.z);
    }

    if (this->Setting_Gizmo_Scale) {
      this->scaleX->point *= scale.x;
      this->scaleY->point *= scale.y;
      this->scaleZ->point *= scale.z;
    }
  }

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}
