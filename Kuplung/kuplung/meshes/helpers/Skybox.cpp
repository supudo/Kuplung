//
//  Skybox.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Skybox.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

const GLenum SkyboxTextureCubemap[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X, // Right
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // Left
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // Top
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // Bottom
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // Back
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, // Front
};

Skybox::~Skybox() {
    glDeleteBuffers(1, &this->vboVertices);

    if (this->vboTexture > 0)
        glDeleteBuffers(1, &this->vboTexture);

    glDisableVertexAttribArray(this->glVS_VertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

#pragma mark - Initialization

void Skybox::init(const int gridSize) {
    this->glUtils = std::make_unique<GLUtils>();

    this->gridSize = gridSize;
    this->Setting_Skybox_Item = 0;

    Skybox_Item si_None;
    si_None.title = "-- No box --";
    si_None.images = { "", "", "", "", "", ""};
    this->skyboxItems.push_back(si_None);

    Skybox_Item si_LakeMountain;
    si_LakeMountain.title = "Lake Mountain";
    si_LakeMountain.images = {
        "lake_mountain_right.jpg",
        "lake_mountain_left.jpg",
        "lake_mountain_top.jpg",
        "lake_mountain_bottom.jpg",
        "lake_mountain_back.jpg",
        "lake_mountain_front.jpg"
    };
    this->skyboxItems.push_back(si_LakeMountain);

    Skybox_Item si_FirePlanet;
    si_FirePlanet.title = "Fire Planet";
    si_FirePlanet.images = {
        "fire_planet_right.jpg",
        "fire_planet_left.jpg",
        "fire_planet_top.jpg",
        "fire_planet_bottom.jpg",
        "fire_planet_back.jpg",
        "fire_planet_front.jpg"
    };
    this->skyboxItems.push_back(si_FirePlanet);

    Skybox_Item si_StormyDays;
    si_StormyDays.title = "Stormy Days";
    si_StormyDays.images = {
        "stormydays_right.jpg",
        "stormydays_left.jpg",
        "stormydays_top.jpg",
        "stormydays_bottom.jpg",
        "stormydays_back.jpg",
        "stormydays_front.jpg"
    };
    this->skyboxItems.push_back(si_StormyDays);
}

bool Skybox::initBuffers() {
    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/skybox.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/skybox.frag";
    std::string shaderSourceFragment = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= this->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[Skybox] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return false;
    }

    this->glVS_MatrixView = this->glUtils->glGetUniform(this->shaderProgram, "vs_MatrixView");
    this->glVS_MatrixProjection = this->glUtils->glGetUniform(this->shaderProgram, "vs_MatrixProjection");
    this->glVS_VertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexPosition");

    if (this->Setting_Skybox_Item > 0) {
        glGenVertexArrays(1, &this->glVAO);
        glBindVertexArray(this->glVAO);

        std::vector<GLfloat> skyboxVertices = {
            // Positions
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
        };

        for (size_t i=0; i<skyboxVertices.size(); i++) {
            skyboxVertices[i] *= gridSize * 10.0;
        }

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(GLfloat), &skyboxVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glVS_VertexPosition);
        glVertexAttribPointer(this->glVS_VertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // skybox textures
        glGenTextures(1, &this->vboTexture);
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->vboTexture);
        for (size_t i=0 ; i<this->skyboxItems[this->Setting_Skybox_Item].images.size(); i++) {
            std::string image = Settings::Instance()->appFolder() + "/skybox/" + this->skyboxItems[this->Setting_Skybox_Item].images[i];

            int tWidth, tHeight, tChannels;
            unsigned char* tPixels = stbi_load(image.c_str(), &tWidth, &tHeight, &tChannels, 0);
            if (!tPixels)
                Settings::Instance()->funcDoLog("Can't load Skybox texture image - " + image + " with error - " + std::string(stbi_failure_reason()));
            else {
                glTexImage2D(SkyboxTextureCubemap[i], 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tPixels);
                stbi_image_free(tPixels);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glBindVertexArray(0);
    }

    return true;
}

#pragma mark - Render

void Skybox::render(const glm::mat4 matrixView, const float plane_close, const float plane_far, const float fov) {
    if (this->glVAO > 0 && this->Setting_Skybox_Item > 0) {
        glBindVertexArray(this->glVAO);
        glUseProgram(this->shaderProgram);

        GLint currentDepthFuncMode;
        glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFuncMode);

        glDepthFunc(GL_LEQUAL);

        glUniformMatrix4fv(this->glVS_MatrixView, 1, GL_FALSE, glm::value_ptr(matrixView));

        glm::mat4 matrixProjection = glm::perspective(fov, (float)Settings::Instance()->SDL_Window_Width / (float)Settings::Instance()->SDL_Window_Height, plane_close, plane_far);
        glUniformMatrix4fv(this->glVS_MatrixProjection, 1, GL_FALSE, glm::value_ptr(matrixProjection));

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->vboTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUseProgram(0);
        glBindVertexArray(0);

        glDepthFunc(currentDepthFuncMode);
    }
}
