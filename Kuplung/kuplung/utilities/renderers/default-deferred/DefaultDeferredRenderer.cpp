//
//  DefaultDeferredRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "DefaultDeferredRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "kuplung/utilities/imgui/imgui_internal.h"
#include "kuplung/utilities/stb/stb_image_write.h"

DefaultDeferredRenderer::DefaultDeferredRenderer(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
}

DefaultDeferredRenderer::~DefaultDeferredRenderer() {
    glDeleteBuffers(1, &this->gPosition);
    glDeleteBuffers(1, &this->gNormal);
    glDeleteBuffers(1, &this->gAlbedoSpec);

    glDisableVertexAttribArray(this->quadVBO);
    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteFramebuffers(1, &this->gBuffer);

    glDeleteProgram(this->shaderProgram_GeometryPass);
    glDeleteProgram(this->shaderProgram_LightingPass);

    for (size_t i=0; i<this->mfLights_Directional.size(); i++) {
        delete this->mfLights_Directional[i];
    }
    for (size_t i=0; i<this->mfLights_Point.size(); i++) {
        delete this->mfLights_Point[i];
    }
    for (size_t i=0; i<this->mfLights_Spot.size(); i++) {
        delete this->mfLights_Spot[i];
    }
    this->glUtils.reset();
}

void DefaultDeferredRenderer::init() {
    this->glUtils = std::make_unique<GLUtils>();
    this->Setting_ReadBuffer = 0;

    this->GLSL_LightSourceNumber_Directional = 8;
    this->GLSL_LightSourceNumber_Point = 4;
    this->GLSL_LightSourceNumber_Spot = 4;

    bool success = true;

    success &= this->initGeometryPass();
    success &= this->initLighingPass();

    if (success) {
        this->initGBuffer();
        this->initLights();
    }
}

void DefaultDeferredRenderer::showSpecificSettings() {
    ImGui::Text("Buffer:");
    const char* buffer_items[] = {
        "Position",
        "Normal",
        "Color + Specular",
        "Lighting"
    };
    ImGui::Combo("##236", &this->Setting_ReadBuffer, buffer_items, IM_ARRAYSIZE(buffer_items));
}

std::string DefaultDeferredRenderer::renderImage(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces) {
    this->fileOutputImage = file;
    std::string endFile;

    int width = Settings::Instance()->SDL_Window_Width;
    int height = Settings::Instance()->SDL_Window_Height;

    glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->renderGBuffer(meshModelFaces);
    this->renderLightingPass();

    unsigned char* pixels = new unsigned char[3 * width * height];

    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    if (this->Setting_ReadBuffer < 3)
        glReadBuffer(GL_COLOR_ATTACHMENT0 + this->Setting_ReadBuffer);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    unsigned char* line_tmp = new unsigned char[3 * width];
    unsigned char* line_a = pixels;
    unsigned char* line_b = pixels + (3 * width * (height - 1));
    while (line_a < line_b) {
        memcpy(line_tmp, line_a, width * 3);
        memcpy(line_a, line_b, width * 3);
        memcpy(line_b, line_tmp, width * 3);
        line_a += width * 3;
        line_b -= width * 3;
    }

    endFile = file.path + ".bmp";
    stbi_write_bmp(endFile.c_str(), width, height, 3, pixels);

    delete[] pixels;
    delete[] line_tmp;

    return endFile;
}

bool DefaultDeferredRenderer::initGeometryPass() {
    // Gemetry Pass
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram_GeometryPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_GeometryPass);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_GeometryPass, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Geometry Pass] Error linking program " + std::to_string(this->shaderProgram_GeometryPass) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_GeometryPass);
        return false;
    }

    this->gl_GeometryPass_Texture_Diffuse = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "texture_diffuse");
    this->gl_GeometryPass_Texture_Specular = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "texture_specular");

    return true;
}

bool DefaultDeferredRenderer::initLighingPass() {
    // Lighting Pass
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram_LightingPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation &= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_LightingPass);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_LightingPass, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Lighting Pass] Error linking program " + std::to_string(this->shaderProgram_LightingPass) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_LightingPass);
        return false;
    }

    return true;
}

void DefaultDeferredRenderer::initGBuffer() {
    // Set up G-Buffer
    // 3 textures:
    // 1. Positions (RGB)
    // 2. Color (RGB) + Specular (A)
    // 3. Normals (RGB)
    glGenFramebuffers(1, &this->gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);

    // - Position color buffer
    glGenTextures(1, &this->gPosition);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->gPosition, 0);

    // - Normal color buffer
    glGenTextures(1, &this->gNormal);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->gNormal, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &this->gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->gAlbedoSpec, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &this->rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[Deferred Rendering T GBuffer] Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DefaultDeferredRenderer::initLights() {
    // light - directional
    for (unsigned int i=0; i<this->GLSL_LightSourceNumber_Directional; i++) {
        ModelFace_LightSource_Directional *f = new ModelFace_LightSource_Directional();
        f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].inUse").c_str());

        f->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].direction").c_str());

        f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].ambient").c_str());
        f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].diffuse").c_str());
        f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].specular").c_str());

        f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].strengthAmbient").c_str());
        f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
        f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("directionalLights[" + std::to_string(i) + "].strengthSpecular").c_str());
        this->mfLights_Directional.push_back(f);
    }

    // light - point
    for (unsigned int i=0; i<this->GLSL_LightSourceNumber_Point; i++) {
        ModelFace_LightSource_Point *f = new ModelFace_LightSource_Point();
        f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].inUse").c_str());
        f->gl_Position = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].position").c_str());

        f->gl_Constant = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].constant").c_str());
        f->gl_Linear = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].linear").c_str());
        f->gl_Quadratic = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].quadratic").c_str());

        f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].ambient").c_str());
        f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].diffuse").c_str());
        f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].specular").c_str());

        f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].strengthAmbient").c_str());
        f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
        f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("pointLights[" + std::to_string(i) + "].strengthSpecular").c_str());
        this->mfLights_Point.push_back(f);
    }

    // light - spot
    for (unsigned int i=0; i<this->GLSL_LightSourceNumber_Spot; i++) {
        ModelFace_LightSource_Spot *f = new ModelFace_LightSource_Spot();
        f->gl_InUse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].inUse").c_str());

        f->gl_Position = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].position").c_str());
        f->gl_Direction = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].direction").c_str());

        f->gl_CutOff = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].cutOff").c_str());
        f->gl_OuterCutOff = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].outerCutOff").c_str());

        f->gl_Constant = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].constant").c_str());
        f->gl_Linear = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].linear").c_str());
        f->gl_Quadratic = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].quadratic").c_str());

        f->gl_Ambient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].ambient").c_str());
        f->gl_Diffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].diffuse").c_str());
        f->gl_Specular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].specular").c_str());

        f->gl_StrengthAmbient = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].strengthAmbient").c_str());
        f->gl_StrengthDiffuse = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].strengthDiffuse").c_str());
        f->gl_StrengthSpecular = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("spotLights[" + std::to_string(i) + "].strengthSpecular").c_str());
        this->mfLights_Spot.push_back(f);
    }
}

void DefaultDeferredRenderer::renderGBuffer(std::vector<ModelFaceBase*> *meshModelFaces) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 1. Geometry Pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->matrixProject = this->managerObjects.matrixProjection;
    this->matrixCamera = this->managerObjects.camera->matrixCamera;
    glm::mat4 matrixModel;
    glUseProgram(this->shaderProgram_GeometryPass);
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "projection"), 1, GL_FALSE, glm::value_ptr(this->matrixProject));
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "view"), 1, GL_FALSE, glm::value_ptr(this->matrixCamera));

    matrixModel = glm::mat4();
    matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));
    matrixModel = glm::rotate(matrixModel, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    matrixModel = glm::translate(matrixModel, glm::vec3(0, 0, 0));

    for (size_t j=0; j<(*meshModelFaces).size(); j++) {
        ModelFaceData* mfd = (ModelFaceData*)(*meshModelFaces)[j];

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

        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "model"), 1, GL_FALSE, glm::value_ptr(matrixModel));

        if (mfd->vboTextureDiffuse > 0 && mfd->meshModel.ModelMaterial.TextureDiffuse.UseTexture) {
            glUniform1i(this->gl_GeometryPass_Texture_Diffuse, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureDiffuse);
        }

        if (mfd->vboTextureSpecular > 0 && mfd->meshModel.ModelMaterial.TextureSpecular.UseTexture) {
            glUniform1i(this->gl_GeometryPass_Texture_Specular, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mfd->vboTextureSpecular);
        }

        mfd->matrixProjection = this->matrixProject;
        mfd->matrixCamera = this->matrixCamera;
        mfd->matrixModel = matrixModel;
        mfd->Setting_ModelViewSkin = this->managerObjects.viewModelSkin;
        mfd->lightSources = this->managerObjects.lightSources;
        mfd->setOptionsSelected(0);
        mfd->setOptionsFOV(this->managerObjects.Setting_FOV);
        mfd->setOptionsOutlineColor(this->managerObjects.Setting_OutlineColor);
        mfd->setOptionsOutlineThickness(this->managerObjects.Setting_OutlineThickness);
        mfd->renderModel(false);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DefaultDeferredRenderer::renderLightingPass() {
    // 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(this->shaderProgram_LightingPass);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    // lights
    unsigned int lightsCount_Directional = 0;
    unsigned int lightsCount_Point = 0;
    unsigned int lightsCount_Spot = 0;
    for (size_t j=0; j<this->managerObjects.lightSources.size(); j++) {
        Light *light = this->managerObjects.lightSources[j];
        switch (light->type) {
            case LightSourceType_Directional: {
                if (lightsCount_Directional < this->GLSL_LightSourceNumber_Directional) {
                    ModelFace_LightSource_Directional *f = this->mfLights_Directional[lightsCount_Directional];

                    glUniform1i(f->gl_InUse, 1);

                    // light
                    glUniform3f(f->gl_Direction, light->matrixModel[2].x, light->matrixModel[2].y, light->matrixModel[2].z);

                    // color
                    glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                    glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                    glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                    // light factors
                    glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                    glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                    glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                    lightsCount_Directional += 1;
                }
                break;
            }
            case LightSourceType_Point: {
                if (lightsCount_Point < this->GLSL_LightSourceNumber_Point) {
                    ModelFace_LightSource_Point *f = this->mfLights_Point[lightsCount_Point];

                    glUniform1i(f->gl_InUse, 1);

                    // light
                    glUniform3f(f->gl_Position, light->matrixModel[3].x, light->matrixModel[3].y, light->matrixModel[3].z);

                    // factors
                    glUniform1f(f->gl_Constant, light->lConstant->point);
                    glUniform1f(f->gl_Linear, light->lLinear->point);
                    glUniform1f(f->gl_Quadratic, light->lQuadratic->point);

                    // color
                    glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                    glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                    glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                    // light factors
                    glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                    glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                    glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                    lightsCount_Point += 1;
                }
                break;
            }
            case LightSourceType_Spot: {
                if (lightsCount_Spot < this->GLSL_LightSourceNumber_Spot) {
                    ModelFace_LightSource_Spot *f = this->mfLights_Spot[lightsCount_Spot];

                    glUniform1i(f->gl_InUse, 1);

                    // light
                    glUniform3f(f->gl_Direction, light->matrixModel[2].x, light->matrixModel[2].y, light->matrixModel[2].z);
                    glUniform3f(f->gl_Position, light->matrixModel[3].x, light->matrixModel[3].y, light->matrixModel[3].z);

                    // cutoff
                    glUniform1f(f->gl_CutOff, glm::cos(glm::radians(light->lCutOff->point)));
                    glUniform1f(f->gl_OuterCutOff, glm::cos(glm::radians(light->lOuterCutOff->point)));

                    // factors
                    glUniform1f(f->gl_Constant, light->lConstant->point);
                    glUniform1f(f->gl_Linear, light->lLinear->point);
                    glUniform1f(f->gl_Quadratic, light->lQuadratic->point);

                    // color
                    glUniform3f(f->gl_Ambient, light->ambient->color.r, light->ambient->color.g, light->ambient->color.b);
                    glUniform3f(f->gl_Diffuse, light->diffuse->color.r, light->diffuse->color.g, light->diffuse->color.b);
                    glUniform3f(f->gl_Specular, light->specular->color.r, light->specular->color.g, light->specular->color.b);

                    // light factors
                    glUniform1f(f->gl_StrengthAmbient, light->ambient->strength);
                    glUniform1f(f->gl_StrengthDiffuse, light->diffuse->strength);
                    glUniform1f(f->gl_StrengthSpecular, light->specular->strength);

                    lightsCount_Spot += 1;
                }
                break;
            }
            default:
                break;
        }
    }

    for (unsigned int j=lightsCount_Directional; j<this->GLSL_LightSourceNumber_Directional; j++) {
        glUniform1i(this->mfLights_Directional[j]->gl_InUse, 0);
    }

    for (unsigned int j=lightsCount_Point; j<this->GLSL_LightSourceNumber_Point; j++) {
        glUniform1i(this->mfLights_Point[j]->gl_InUse, 0);
    }

    for (unsigned int j=lightsCount_Spot; j<this->GLSL_LightSourceNumber_Spot; j++) {
        glUniform1i(this->mfLights_Spot[j]->gl_InUse, 0);
    }

    glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, "viewPos"), 1, &this->managerObjects.camera->cameraPosition[0]);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "draw_mode"), this->managerObjects.Setting_LightingPass_DrawMode + 1);
    glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, "ambientStrength"), this->managerObjects.Setting_DeferredAmbientStrength);
    glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, "gammaCoeficient"), this->managerObjects.Setting_GammaCoeficient);
    this->renderQuad();
}

void DefaultDeferredRenderer::renderQuad() {
    if (this->quadVAO == 0) {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &this->quadVAO);
        glGenBuffers(1, &this->quadVBO);
        glBindVertexArray(this->quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, this->quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(this->quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
