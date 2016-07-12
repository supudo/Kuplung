//
//  RenderingDeferred.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingDeferred.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

RenderingDeferred::~RenderingDeferred() {
    this->destroy();
}

void RenderingDeferred::destroy() {
    glDeleteBuffers(1, &this->gPosition);
    glDeleteBuffers(1, &this->gNormal);
    glDeleteBuffers(1, &this->gAlbedoSpec);

    glDisableVertexAttribArray(this->quadVBO);
    glDisableVertexAttribArray(this->cubeVBO);

    glDeleteVertexArrays(1, &this->quadVAO);
    glDeleteVertexArrays(1, &this->cubeVAO);

    glDeleteFramebuffers(1, &this->gBuffer);

    glDeleteProgram(this->shaderProgram_GeometryPass);
    glDeleteProgram(this->shaderProgram_LightingPass);
    glDeleteProgram(this->shaderProgram_LightBox);

    for (size_t i=0; i<this->mfLights_Directional.size(); i++) {
        delete this->mfLights_Directional[i];
    }
    for (size_t i=0; i<this->mfLights_Point.size(); i++) {
        delete this->mfLights_Point[i];
    }
    for (size_t i=0; i<this->mfLights_Spot.size(); i++) {
        delete this->mfLights_Spot[i];
    }
}

bool RenderingDeferred::init() {
    this->glUtils = new GLUtils();

    bool success = true;

    success |= this->initGeometryPass();
    success |= this->initLighingPass();
    success |= this->initLightObjects();

    this->initProps();
    this->initGBuffer();
    this->initLights();

    return success;
}

bool RenderingDeferred::initGeometryPass() {
    // Gemetry Pass
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram_GeometryPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

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
    return true;
}

bool RenderingDeferred::initLighingPass() {
    // Lighting Pass
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram_LightingPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

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

bool RenderingDeferred::initLightObjects() {
    // Light Boxes
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());

    this->shaderProgram_LightBox = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_VERTEX_SHADER, shaderSourceVertex.c_str());
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_FRAGMENT_SHADER, shaderSourceFragment.c_str());

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram_LightBox);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram_LightBox, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Deferred - Light Box] Error linking program " + std::to_string(this->shaderProgram_LightBox) + "!");
        this->glUtils->printProgramLog(this->shaderProgram_LightBox);
        return false;
    }
    return true;
}

void RenderingDeferred::initProps() {
    glUseProgram(this->shaderProgram_LightingPass);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "gAlbedoSpec"), 2);

    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

//    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
//    this->objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
//    this->objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
//    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
//    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
//    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
//    this->objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
//    this->objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
//    this->objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

    // - Colors
    srand(13);
    for (GLuint i = 0; i < this->NR_LIGHTS; i++) {
        // Calculate slightly random offsets
        GLfloat xPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
        GLfloat yPos = ((rand() % 100) / 100.0f) * 6.0f - 4.0f;
        GLfloat zPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
        this->lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

        // Also calculate random color
        GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
        GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
        GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
        this->lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }
}

void RenderingDeferred::initGBuffer() {
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
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Settings::Instance()->funcDoLog("[Deferred Rendering T GBuffer] Framebuffer not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void RenderingDeferred::initLights() {
    // light - directional
    for (int i=0; i<this->GLSL_LightSourceNumber_Directional; i++) {
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
    for (int i=0; i<this->GLSL_LightSourceNumber_Point; i++) {
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
    for (int i=0; i<this->GLSL_LightSourceNumber_Spot; i++) {
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

void RenderingDeferred::render(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects) {
    this->renderGBuffer(meshModelFaces, managerObjects);
    this->renderLightingPass(managerObjects);
    this->renderLightObjects();
}

void RenderingDeferred::renderGBuffer(std::vector<ModelFaceData*> meshModelFaces, ObjectsManager *managerObjects) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // 1. Geometry Pass: render scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->matrixProject = managerObjects->matrixProjection;
    this->matrixCamera = managerObjects->camera->matrixCamera;
    glm::mat4 model;
    glUseProgram(this->shaderProgram_GeometryPass);
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "projection"), 1, GL_FALSE, glm::value_ptr(this->matrixProject));
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "view"), 1, GL_FALSE, glm::value_ptr(this->matrixCamera));
    for (GLuint i=0; i<this->objectPositions.size(); i++) {
        model = glm::mat4();
        model = glm::translate(model, this->objectPositions[i]);
        model = glm::scale(model, glm::vec3(0.25f));

        model = glm::translate(model, glm::vec3(0, 0, 0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
        model = glm::translate(model, glm::vec3(0, 0, 0));

        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_GeometryPass, "model"), 1, GL_FALSE, glm::value_ptr(model));

        for (size_t j=0; j<meshModelFaces.size(); j++) {
            meshModelFaces[j]->matrixProjection = this->matrixProject;
            meshModelFaces[j]->matrixCamera = this->matrixCamera;
            meshModelFaces[j]->matrixModel = model;
            meshModelFaces[j]->Setting_ModelViewSkin = managerObjects->viewModelSkin;
            meshModelFaces[j]->lightSources = managerObjects->lightSources;
            meshModelFaces[j]->setOptionsFOV(managerObjects->Setting_FOV);
            meshModelFaces[j]->setOptionsOutlineColor(managerObjects->Setting_OutlineColor);
            meshModelFaces[j]->setOptionsOutlineThickness(managerObjects->Setting_OutlineThickness);
            meshModelFaces[j]->renderModel(false);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderingDeferred::renderLightingPass(ObjectsManager *managerObjects) {
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
    int lightsCount_Directional = 0;
    int lightsCount_Point = 0;
    int lightsCount_Spot = 0;
    for (int j=0; j<(int)managerObjects->lightSources.size(); j++) {
        Light *light = managerObjects->lightSources[j];
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

    for (int j=lightsCount_Directional; j<this->GLSL_LightSourceNumber_Directional; j++) {
        glUniform1i(this->mfLights_Directional[j]->gl_InUse, 0);
    }

    for (int j=lightsCount_Point; j<this->GLSL_LightSourceNumber_Point; j++) {
        glUniform1i(this->mfLights_Point[j]->gl_InUse, 0);
    }

    for (int j=lightsCount_Spot; j<this->GLSL_LightSourceNumber_Spot; j++) {
        glUniform1i(this->mfLights_Spot[j]->gl_InUse, 0);
    }

    // Also send light relevant uniforms
    for (GLuint i=0; i<this->lightPositions.size(); i++) {
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Position").c_str()), 1, &this->lightPositions[i][0]);
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Color").c_str()), 1, &this->lightColors[i][0]);

        // Update attenuation parameters and calculate radius
        const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
        const GLfloat linear = 0.7f;
        const GLfloat quadratic = 1.8f;
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Linear").c_str()), linear);
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Quadratic").c_str()), quadratic);

        // Then calculate radius of light volume/sphere
        const GLfloat lightThreshold = 5.0; // 5 / 256
        const GLfloat maxBrightness = std::fmaxf(std::fmaxf(this->lightColors[i].r, this->lightColors[i].g), this->lightColors[i].b);
        GLfloat radius = (-linear + static_cast<float>(std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / lightThreshold) * maxBrightness)))) / (2 * quadratic);
        glUniform1f(glGetUniformLocation(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Radius").c_str()), radius);
    }
    glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, "viewPos"), 1, &managerObjects->camera->cameraPosition[0]);
    glUniform1i(glGetUniformLocation(this->shaderProgram_LightingPass, "draw_mode"), managerObjects->Setting_LightingPass_DrawMode + 1);
    this->renderQuad();
}

void RenderingDeferred::renderLightObjects() {
    // 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      0, 0,
                      Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Render lights on top of scene, by blitting
    glUseProgram(this->shaderProgram_LightBox);
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "projection"), 1, GL_FALSE, glm::value_ptr(this->matrixProject));
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "view"), 1, GL_FALSE, glm::value_ptr(this->matrixCamera));
    for (GLuint i=0; i<this->lightPositions.size(); i++) {
        glm::mat4 model = glm::mat4();
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.25f));
        glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram_LightBox, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(this->shaderProgram_LightBox, "lightColor"), 1, &lightColors[i][0]);
        this->renderCube();
    }
}

void RenderingDeferred::renderQuad() {
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


void RenderingDeferred::renderCube() {
    if (this->cubeVAO == 0) {
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
        };
        glGenVertexArrays(1, &this->cubeVAO);
        glGenBuffers(1, &this->cubeVBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(this->cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // Render Cube
    glBindVertexArray(this->cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
