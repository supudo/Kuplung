//
//  Terrain.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Terrain.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

Terrain::~Terrain() {
  glDeleteBuffers(1, &this->vboVertices);
  glDeleteBuffers(1, &this->vboNormals);
  glDeleteBuffers(1, &this->vboTextureCoordinates);
  glDeleteBuffers(1, &this->vboColors);
  glDeleteBuffers(1, &this->vboIndices);

  glDisableVertexAttribArray(this->glAttributeVertexPosition);
  glDisableVertexAttribArray(this->glAttributeTextureCoord);
  glDisableVertexAttribArray(this->glAttributeVertexNormal);

  glDetachShader(this->shaderProgram, this->shaderVertex);
  glDetachShader(this->shaderProgram, this->shaderFragment);
  glDeleteProgram(this->shaderProgram);

  glDeleteShader(this->shaderVertex);
  glDeleteShader(this->shaderFragment);

  glDeleteVertexArrays(1, &this->glVAO);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}

#pragma mark - Initialization

Terrain::Terrain()
    : terrainGenerator(std::make_unique<HeightmapGenerator>())
    , Setting_UseTexture(false)
    , Setting_Wireframe(false) {
  this->terrainGenerator->initPosition();
}

#pragma mark - Public

bool Terrain::initShaderProgram() {
  bool success = true;

  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/terrain.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  shaderPath = Settings::Instance()->appFolder() + "/shaders/terrain.frag";
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
    Settings::Instance()->funcDoLog("[Terrain] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return success = false;
  }
  else {
    this->glAttributeVertexPosition = Settings::Instance()->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
    this->glAttributeVertexNormal = Settings::Instance()->glUtils->glGetAttribute(this->shaderProgram, "a_vertexNormal");
    this->glAttributeTextureCoord = Settings::Instance()->glUtils->glGetAttribute(this->shaderProgram, "a_textureCoord");
    this->glUniformHasTexture = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "has_texture");
    this->glUniformSamplerTexture = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "sampler_texture");
    this->glAttributeColor = Settings::Instance()->glUtils->glGetAttribute(this->shaderProgram, "a_color");
    this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
  }

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));

  return success;
}

void Terrain::initBuffers(std::string const& assetsFolder, const int width, const int height) {
  glGenVertexArrays(1, &this->glVAO);
  glBindVertexArray(this->glVAO);

  this->terrainGenerator->generateTerrain(assetsFolder, width, height);
  this->heightmapImage = this->terrainGenerator->heightmapImage;

  // vertices
  glGenBuffers(1, &this->vboVertices);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->terrainGenerator->vertices.size() * sizeof(glm::vec3)), &this->terrainGenerator->vertices[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(this->glAttributeVertexPosition);
  glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

  // normals
  glGenBuffers(1, &this->vboNormals);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->terrainGenerator->normals.size() * sizeof(glm::vec3)), &this->terrainGenerator->normals[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(this->glAttributeVertexNormal);
  glVertexAttribPointer(this->glAttributeVertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

  // texture
  glGenBuffers(1, &this->vboTextureCoordinates);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->terrainGenerator->uvs.size() * sizeof(glm::vec2)), &this->terrainGenerator->uvs[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(this->glAttributeTextureCoord);
  glVertexAttribPointer(this->glAttributeTextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

  std::string matImageLocal = this->terrainGenerator->heightmapImage;
  int tWidth, tHeight, tChannels;
  unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
  if (!tPixels)
    Settings::Instance()->funcDoLog("Can't load terrain texture image - " + matImageLocal + " with error - " + std::string(stbi_failure_reason()));
  else {
    glGenTextures(1, &this->vboTextureDiffuse);
    glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLint textureFormat = 0;
    switch (tChannels) {
      case 1:
        textureFormat = GL_LUMINANCE;
        break;
      case 2:
        textureFormat = GL_LUMINANCE_ALPHA;
        break;
      case 3:
        textureFormat = GL_RGB;
        break;
      case 4:
        textureFormat = GL_RGBA;
        break;
      default:
        textureFormat = GL_RGB;
        break;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat, tWidth, tHeight, 0, textureFormat, GL_UNSIGNED_BYTE, tPixels);
    stbi_image_free(tPixels);
  }

  // colors
  glGenBuffers(1, &this->vboColors);
  glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
  glBufferData(GL_ARRAY_BUFFER, static_cast<GLuint>(this->terrainGenerator->colors.size() * sizeof(glm::vec3)), &this->terrainGenerator->colors[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(this->glAttributeColor);
  glVertexAttribPointer(this->glAttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

  // indices
  glGenBuffers(1, &this->vboIndices);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(this->terrainGenerator->indices.size() * sizeof(GLuint)), &this->terrainGenerator->indices[0], GL_STATIC_DRAW);

  glBindVertexArray(0);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}

#pragma mark - Render

void Terrain::render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const glm::mat4& matrixModel) {
  if (this->glVAO > 0) {
    glUseProgram(this->shaderProgram);

    glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
    glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glUniform1i(this->glUniformHasTexture, this->Setting_UseTexture);
    glUniform1i(this->glUniformSamplerTexture, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->vboTextureDiffuse);

    // draw
    glBindVertexArray(this->glVAO);

    if (this->Setting_Wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, static_cast<int>(this->terrainGenerator->indices.size()), GL_UNSIGNED_INT, nullptr);

    if (this->Setting_Wireframe)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(0);

    glUseProgram(0);
  }

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}
