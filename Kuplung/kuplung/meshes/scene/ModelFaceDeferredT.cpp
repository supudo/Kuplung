//
//  ModelFaceDeferredT.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ModelFaceDeferredT.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <boost/filesystem.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

void ModelFaceDeferredT::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboNormals);
    glDeleteBuffers(1, &this->vboTextureCoordinates);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboTangents);
    glDeleteBuffers(1, &this->vboBitangents);

    GLint maxColorAttachments = 1;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
    GLint colorAttachment;
    GLenum att = GL_COLOR_ATTACHMENT0;
    for (colorAttachment = 0; colorAttachment < maxColorAttachments; colorAttachment++) {
        att += colorAttachment;
        GLint param;
        GLuint objName;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &param);
        if (GL_RENDERBUFFER == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteRenderbuffers(1, &objName);
        }
        else if (GL_TEXTURE == param) {
            glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, att, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &param);
            objName = ((GLuint*)(&param))[0];
            glDeleteTextures(1, &objName);
        }
    }

    glDisableVertexAttribArray(this->gl_GeometryPass_Position);
    glDisableVertexAttribArray(this->gl_GeometryPass_Normal);
    glDisableVertexAttribArray(this->gl_GeometryPass_TextureCoordinate);

    glDeleteProgram(this->shaderProgram_GeometryPass);
    glDeleteProgram(this->shaderProgram_LightingPass);
    glDeleteProgram(this->shaderProgram_LightBox);
}

bool ModelFaceDeferredT::initShaderProgram() {
    bool success = true;

    success |= this->initShader_GeometryPass();
    success |= this->initShader_LightingPass();
    success |= this->initShader_LightBox();

    if (success) {
        // Set samplers
//        glUseProgram(this->shaderProgram_LightingPass);
//        glUniform1i(this->gl_LightingPass_Position, 0);
//        glUniform1i(this->gl_LightingPass_Normal, 1);
//        glUniform1i(this->gl_LightingPass_AlbedoSpec, 2);

        // - Colors
        srand(13);
        for (GLuint i=0; i<this->NR_LIGHTS; i++) {
            // Calculate slightly random offsets
            GLfloat xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            GLfloat yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
            GLfloat zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
            this->lightPositions.push_back(glm::vec3(xPos, yPos, zPos));

            // Also calculate random color
            GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
            GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
            GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5; // Between 0.5 and 1.0
            this->lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }

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

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Settings::Instance()->funcDoLog("[Deferred Rendering GBuffer] Framebuffer not complete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    return success;
}

bool ModelFaceDeferredT::initShader_GeometryPass() {
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.vert";
    std::string shaderSourceVertex = ModelFace::readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_g_buffer.frag";
    std::string shaderSourceFragment = ModelFace::readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram_GeometryPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_GeometryPass, GL_FRAGMENT_SHADER, shader_fragment);

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
    else {
        this->gl_GeometryPass_Position = this->glUtils->glGetAttribute(this->shaderProgram_GeometryPass, "position");
        this->gl_GeometryPass_Normal = this->glUtils->glGetAttribute(this->shaderProgram_GeometryPass, "normal");
        this->gl_GeometryPass_TextureCoordinate = this->glUtils->glGetAttribute(this->shaderProgram_GeometryPass, "texCoords");

        this->gl_GeometryPass_ProjectionMatrix = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "projection");
        this->gl_GeometryPass_ViewMatrix = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "view");
        this->gl_GeometryPass_ModelMatrix = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "model");

        this->gl_GeometryPass_TextureDiffuse = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "texture_diffuse1");
        this->gl_GeometryPass_TextureSpecular = this->glUtils->glGetUniform(this->shaderProgram_GeometryPass, "texture_specular1");
    }

    return true;
}

bool ModelFaceDeferredT::initShader_LightingPass() {
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.vert";
    std::string shaderSourceVertex = ModelFace::readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_shading.frag";
    std::string shaderSourceFragment = ModelFace::readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram_LightingPass = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightingPass, GL_FRAGMENT_SHADER, shader_fragment);

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
    else {
        this->gl_LightingPass_VPosition = this->glUtils->glGetAttribute(this->shaderProgram_LightingPass, "position");
        this->gl_LightingPass_VTexCoords = this->glUtils->glGetAttribute(this->shaderProgram_LightingPass, "texCoords");

        this->gl_LightingPass_Position = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, "gPosition");
        this->gl_LightingPass_Normal = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, "gNormal");
        this->gl_LightingPass_AlbedoSpec = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, "gAlbedoSpec");

        this->gl_LightingPass_ViewPosition = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, "viewPos");
        this->gl_LightingPass_DrawMode = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, "draw_mode");

        for (GLuint i = 0; i < this->NR_LIGHTS; i++) {
            ModelDeferred_LightSource source;
            source.gl_Position = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Position").c_str());
            source.gl_Color = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Color").c_str());
            source.gl_Linear = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Linear").c_str());
            source.gl_Quadratic = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Quadratic").c_str());
            source.gl_Radius = this->glUtils->glGetUniform(this->shaderProgram_LightingPass, ("lights[" + std::to_string(i) + "].Radius").c_str());
            this->lightSources.push_back(source);
        }
    }

    return true;
}

bool ModelFaceDeferredT::initShader_LightBox() {
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.vert";
    std::string shaderSourceVertex = ModelFace::readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/deferred_light_box.frag";
    std::string shaderSourceFragment = ModelFace::readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram_LightBox = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram_LightBox, GL_FRAGMENT_SHADER, shader_fragment);

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
    else {
        this->gl_LightBox_ProjectionMatrix = this->glUtils->glGetUniform(this->shaderProgram_LightBox, "projection");
        this->gl_LightBox_ViewMatrix = this->glUtils->glGetUniform(this->shaderProgram_LightBox, "view");
        this->gl_LightBox_ModelMatrix = this->glUtils->glGetUniform(this->shaderProgram_LightBox, "model");
        this->gl_LightBox_LightColor = this->glUtils->glGetUniform(this->shaderProgram_LightBox, "lightColor");
    }

    return true;
}

void ModelFaceDeferredT::initBuffers(std::string assetsFolder) {
    this->assetsFolder = assetsFolder;

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.vertices.size() * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->gl_GeometryPass_Position);
    glVertexAttribPointer(this->gl_GeometryPass_Position, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.normals.size() * sizeof(glm::vec3), &this->meshModel.normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->gl_GeometryPass_Normal);
    glVertexAttribPointer(this->gl_GeometryPass_Normal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // textures and colors
    if (this->meshModel.texture_coordinates.size() > 0) {
        glGenBuffers(1, &this->vboTextureCoordinates);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
        glBufferData(GL_ARRAY_BUFFER, this->meshModel.texture_coordinates.size() * sizeof(glm::vec2), &this->meshModel.texture_coordinates[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->gl_GeometryPass_Position);
        glVertexAttribPointer(this->gl_GeometryPass_Position, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        ModelFace::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureDiffuse, objMaterialImageType_Diffuse, &this->gl_GeometryPass_TextureDiffuse);
        ModelFace::loadTexture(this->assetsFolder, this->meshModel.ModelMaterial.TextureSpecular, objMaterialImageType_Specular, &this->gl_GeometryPass_TextureSpecular);
    }

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    this->initBuffersAgain = false;
}

void ModelFaceDeferredT::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel, glm::vec3 vecCameraPosition, WorldGrid *grid, glm::vec3 uiAmbientLight) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
    this->matrixModel = matrixModel;
    this->vecCameraPosition = vecCameraPosition;
    this->grid = grid;
    this->uiAmbientLight = uiAmbientLight;

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    this->renderGeometryPass();
    this->renderLightingPass();
    this->renderLightBox();
}

void ModelFaceDeferredT::renderGeometryPass() {
//    glDepthMask(GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(this->shaderProgram_GeometryPass);

    glUniformMatrix4fv(this->gl_GeometryPass_ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixProjection));
    glUniformMatrix4fv(this->gl_GeometryPass_ViewMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixCamera));

    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(1.0, 1.0, 1.0));
    model = glm::scale(model, glm::vec3(0.25f));
    glUniformMatrix4fv(this->gl_GeometryPass_ModelMatrix, 1, GL_FALSE, glm::value_ptr(model));

    if (this->gl_GeometryPass_TextureDiffuse) {
        glUniform1i(this->gl_GeometryPass_TextureDiffuse, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->gl_GeometryPass_TextureDiffuse);
    }

    if (this->gl_GeometryPass_TextureSpecular) {
        glUniform1i(this->gl_GeometryPass_TextureSpecular, 2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->gl_GeometryPass_TextureSpecular);
    }

    glBindVertexArray(this->glVAO);
    glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDepthMask(GL_FALSE);
}

void ModelFaceDeferredT::renderLightingPass() {
    // 2. Lighting Pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(this->shaderProgram_LightingPass);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    // Also send light relevant uniforms
    for (GLuint i = 0; i < this->lightPositions.size(); i++) {
        glUniform3fv(this->lightSources[i].gl_Position, 1, &this->lightPositions[i][0]);
        glUniform3fv(this->lightSources[i].gl_Color, 1, &this->lightColors[i][0]);

        // Update attenuation parameters and calculate radius
        const GLfloat constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
        const GLfloat linear = 0.7;
        const GLfloat quadratic = 1.8;
        glUniform1f(this->lightSources[i].gl_Linear, linear);
        glUniform1f(this->lightSources[i].gl_Quadratic, quadratic);

        // Then calculate radius of light volume/sphere
        const GLfloat lightThreshold = 5.0; // 5 / 256
        const GLfloat maxBrightness = std::fmaxf(std::fmaxf(this->lightColors[i].r, this->lightColors[i].g), this->lightColors[i].b);
        GLfloat radius = (-linear + static_cast<float>(std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0 / lightThreshold) * maxBrightness)))) / (2 * quadratic);
        glUniform1f(this->lightSources[i].gl_Radius, radius);
    }
//    glUniform3fv(glGetUniformLocation(this->shaderProgram_LightingPass, "viewPos"), 1, &camera.Position[0]);
    glUniform3fv(this->gl_LightingPass_ViewPosition, 1, &this->vecCameraPosition[0]);
    glUniform1i(this->gl_LightingPass_DrawMode, this->Setting_LightingPass_DrawMode + 1);
    this->renderQuad();
}

void ModelFaceDeferredT::renderLightBox() {
    // 2.5. Copy content of geometry's depth buffer to default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->gBuffer);
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
    glUniformMatrix4fv(this->gl_LightBox_ProjectionMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixProjection));
    glUniformMatrix4fv(this->gl_LightBox_ViewMatrix, 1, GL_FALSE, glm::value_ptr(this->matrixCamera));
    glm::mat4 matrixModel;
    for (GLuint i=0; i<this->lightPositions.size(); i++) {
        matrixModel = glm::mat4();
        matrixModel = glm::translate(matrixModel, this->lightPositions[i]);
        matrixModel = glm::scale(matrixModel, glm::vec3(0.25f));
        glUniformMatrix4fv(this->gl_LightBox_ModelMatrix, 1, GL_FALSE, glm::value_ptr(matrixModel));
        glUniform3fv(this->gl_LightBox_LightColor, 1, &this->lightColors[i][0]);
        this->renderCube();
    }
}

void ModelFaceDeferredT::renderQuad() {
    if (this->vaoQuad == 0) {
        GLfloat quadVertices[] = {
            // Positions        // Texture Coords
            -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        // Setup plane VAO
        glGenVertexArrays(1, &this->vaoQuad);
        glGenBuffers(1, &this->vboQuad);
        glBindVertexArray(this->vaoQuad);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboQuad);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(this->vaoQuad);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void ModelFaceDeferredT::renderCube() {
    // Initialize (if necessary)
    if (this->vaoCube == 0)
    {
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
        glGenVertexArrays(1, &this->vaoCube);
        glGenBuffers(1, &this->vboCube);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, this->vboCube);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(this->vaoCube);
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
    glBindVertexArray(this->vaoCube);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
