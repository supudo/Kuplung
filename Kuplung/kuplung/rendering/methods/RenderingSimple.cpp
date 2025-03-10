//
//  RenderingSimple.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingSimple.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

RenderingSimple::RenderingSimple(ObjectsManager& managerObjects) : managerObjects(managerObjects) {}

RenderingSimple::~RenderingSimple() {
  glDeleteProgram(this->shaderProgram);
}

bool RenderingSimple::init() {
  // vertex shader
  std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.vert";
  std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_vertex = shaderSourceVertex.c_str();

  // tessellation control shader
  shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.tcs";
  std::string shaderSourceTCS = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_tess_control = shaderSourceTCS.c_str();

  // tessellation evaluation shader
  shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.tes";
  std::string shaderSourceTES = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_tess_eval = shaderSourceTES.c_str();

  // geometry shader
  shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.geom";
  std::string shaderSourceGeometry = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_geometry = shaderSourceGeometry.c_str();

  // fragment shader
  shaderPath = Settings::Instance()->appFolder() + "/shaders/rendering_simple.frag";
  std::string shaderSourceFragment = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
  const char* shader_fragment = shaderSourceFragment.c_str();

  this->shaderProgram = glCreateProgram();

  bool shaderCompilation = true;
  shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_VERTEX_SHADER, shader_vertex);
  shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_TESS_CONTROL_SHADER, shader_tess_control);
  shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_TESS_EVALUATION_SHADER, shader_tess_eval);
  shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_GEOMETRY_SHADER, shader_geometry);
  shaderCompilation &= Settings::Instance()->glUtils->compileShader(this->shaderProgram, GL_FRAGMENT_SHADER, shader_fragment);

  if (!shaderCompilation)
    return false;

  glLinkProgram(this->shaderProgram);

  GLint programSuccess = GL_TRUE;
  glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
  if (programSuccess != GL_TRUE) {
    Settings::Instance()->funcDoLog("[RenderingSimple - initShaders] Error linking program " + std::to_string(this->shaderProgram) + "!");
    Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
    return false;
  }
  else {
#ifdef Def_Kuplung_OpenGL_4x
    glPatchParameteri(GL_PATCH_VERTICES, 3);
#endif

    this->glVS_MVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "vs_MVPMatrix");
    this->glVS_WorldMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "vs_WorldMatrix");

    this->glFS_HasSamplerTexture = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "has_texture");
    this->glFS_SamplerTexture = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "sampler_texture");

    this->glFS_CameraPosition = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_cameraPosition");
    this->glFS_UIAmbient = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_UIAmbient");

    this->solidLight = std::make_unique<ModelFace_LightSource_Directional>();
    this->solidLight->gl_InUse = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.inUse");
    this->solidLight->gl_Direction = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.direction");
    this->solidLight->gl_Ambient = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.ambient");
    this->solidLight->gl_Diffuse = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.diffuse");
    this->solidLight->gl_Specular = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.specular");
    this->solidLight->gl_StrengthAmbient = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthAmbient");
    this->solidLight->gl_StrengthDiffuse = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthDiffuse");
    this->solidLight->gl_StrengthSpecular = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "solidSkin_Light.strengthSpecular");
  }

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
  return true;
}

void RenderingSimple::render(const std::vector<ModelFaceData*>& meshModelFaces, const int& selectedModel) {
  glViewport(0, 0, Settings::Instance()->SDL_DrawableSize_Width, Settings::Instance()->SDL_DrawableSize_Height);

  this->matrixProjection = this->managerObjects.matrixProjection;
  this->matrixCamera = this->managerObjects.camera->matrixCamera;
  this->vecCameraPosition = this->managerObjects.camera->cameraPosition;
  this->uiAmbientLight = this->managerObjects.Setting_UIAmbientLight;

  glUseProgram(this->shaderProgram);

  for (size_t i = 0; i < meshModelFaces.size(); i++) {
    ModelFaceData* mfd = meshModelFaces[i];

    glm::mat4 matrixModel = glm::mat4(1.0);
    matrixModel *= this->managerObjects.grid->matrixModel;
    // scale
    matrixModel = glm::scale(matrixModel, glm::vec3(mfd->scaleX->point, mfd->scaleY->point, mfd->scaleZ->point));
    // rotate
    matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
    matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateX->point), glm::vec3(1, 0, 0));
    matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateY->point), glm::vec3(0, 1, 0));
    matrixModel = glm::rotate(matrixModel, glm::radians(mfd->rotateZ->point), glm::vec3(0, 0, 1));
    matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
    // translate
    matrixModel = glm::translate(matrixModel, glm::vec3(mfd->positionX->point, mfd->positionY->point, mfd->positionZ->point));

    mfd->matrixProjection = this->matrixProjection;
    mfd->matrixCamera = this->matrixCamera;
    mfd->matrixModel = matrixModel;
    mfd->Setting_ModelViewSkin = this->managerObjects.viewModelSkin;
    mfd->lightSources = this->managerObjects.lightSources;
    mfd->setOptionsFOV(this->managerObjects.Setting_FOV);
    mfd->setOptionsOutlineColor(this->managerObjects.Setting_OutlineColor);
    mfd->setOptionsOutlineThickness(this->managerObjects.Setting_OutlineThickness);

    glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * matrixModel;
    glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

    glUniformMatrix4fv(this->glVS_WorldMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));

    glUniform3f(this->glFS_CameraPosition, this->vecCameraPosition.x, this->vecCameraPosition.y, this->vecCameraPosition.z);
    glUniform3f(this->glFS_UIAmbient, this->uiAmbientLight.r, this->uiAmbientLight.g, this->uiAmbientLight.b);

    glUniform1i(this->solidLight->gl_InUse, 1);
    glUniform3f(this->solidLight->gl_Direction, this->managerObjects.SolidLight_Direction.x, this->managerObjects.SolidLight_Direction.y, this->managerObjects.SolidLight_Direction.z);
    glUniform3f(this->solidLight->gl_Ambient, this->managerObjects.SolidLight_Ambient.r, this->managerObjects.SolidLight_Ambient.g, this->managerObjects.SolidLight_Ambient.b);
    glUniform3f(this->solidLight->gl_Diffuse, this->managerObjects.SolidLight_Diffuse.r, this->managerObjects.SolidLight_Diffuse.g, this->managerObjects.SolidLight_Diffuse.b);
    glUniform3f(this->solidLight->gl_Specular, this->managerObjects.SolidLight_Specular.r, this->managerObjects.SolidLight_Specular.g, this->managerObjects.SolidLight_Specular.b);
    glUniform1f(this->solidLight->gl_StrengthAmbient, this->managerObjects.SolidLight_Ambient_Strength);
    glUniform1f(this->solidLight->gl_StrengthDiffuse, this->managerObjects.SolidLight_Diffuse_Strength);
    glUniform1f(this->solidLight->gl_StrengthSpecular, this->managerObjects.SolidLight_Specular_Strength);

    if (mfd->vboTextureDiffuse > 0 && mfd->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
      glUniform1i(this->glFS_HasSamplerTexture, 1);
      glUniform1i(this->glFS_SamplerTexture, 0);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDiffuse);
    }
    else
      glUniform1i(this->glFS_HasSamplerTexture, 0);

    mfd->renderModel(true);
  }

  glUseProgram(0);

  Settings::Instance()->glUtils->CheckForGLErrors(Settings::Instance()->string_format("%s - %s", __FILE__, __func__));
}
