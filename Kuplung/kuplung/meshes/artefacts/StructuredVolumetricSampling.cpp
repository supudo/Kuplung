//
//  StructuredVolumetricSampling.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "StructuredVolumetricSampling.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

void StructuredVolumetricSampling::destroy() {
    glDeleteBuffers(1, &this->vboVertices);

    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDeleteTextures(1, &this->vboTextureNoise);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

void StructuredVolumetricSampling::init() {
    this->glUtils = std::make_unique<GLUtils>();
}

bool StructuredVolumetricSampling::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/structured_vol_sampling.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/structured_vol_sampling.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");

        this->glFS_deltaRunningTime = this->glUtils->glGetUniform(this->shaderProgram, "fs_deltaRunningTime");
        this->glFS_noiseTextureSampler = this->glUtils->glGetUniform(this->shaderProgram, "fs_noiseTextureSampler");
        this->glFS_screenResolution = this->glUtils->glGetUniform(this->shaderProgram, "fs_screenResolution");
        this->glFS_mouseCoordinates = this->glUtils->glGetUniform(this->shaderProgram, "fs_mouseCoordinates");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void StructuredVolumetricSampling::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    GLfloat vertices[] = {
        -1.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  0.0f,
        -1.0f, -1.0f,  0.0f
    };

    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    std::string matImageLocal = Settings::Instance()->appFolder() + "/noise16.png";
    int tWidth, tHeight, tChannels;
    unsigned char* tPixels = stbi_load(matImageLocal.c_str(), &tWidth, &tHeight, &tChannels, 0);
    if (tPixels) {
        glGenTextures(1, &this->vboTextureNoise);
        glBindTexture(GL_TEXTURE_2D, this->vboTextureNoise);
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

    glBindVertexArray(0);
}

void StructuredVolumetricSampling::render(int mouseX, int mouseY, float seconds) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        glEnable(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(this->glFS_noiseTextureSampler, 0);

        glUniform1f(this->glFS_deltaRunningTime, seconds);
        glUniform3f(this->glFS_screenResolution, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, 0.0f);
        glUniform4f(this->glFS_mouseCoordinates, float(mouseX), float(mouseY), 0.0f, 0.0f);

        // draw
        glBindVertexArray(this->glVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}
