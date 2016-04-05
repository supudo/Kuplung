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

#pragma mark - Destroy

Skybox::~Skybox() {
    this->destroy();
}

void Skybox::destroy() {
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
}

#pragma mark - Initialization

bool Skybox::init() {
    this->glUtils = new GLUtils();

    // vertex shader
    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/skybox.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    // fragment shader
    shaderPath = Settings::Instance()->appFolder() + "/shaders/skybox.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation |= this->glUtils->compileShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        this->glUtils->printProgramLog(this->shaderProgram);
        return false;
    }

    this->glVS_MVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "vs_WorldMatrix");
    this->glVS_VertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "vs_vertexPosition");

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
        skyboxVertices[i] *= 10.0;
    }

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(GLfloat), &skyboxVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glVS_VertexPosition);
    glVertexAttribPointer(this->glVS_VertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // skybox textures
    std::vector<std::string> skybox_images = {
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_right.jpg",
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_left.jpg",
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_top.jpg",
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_bottom.jpg",
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_back.jpg",
        Settings::Instance()->appFolder() + "/skybox/lake_mountain_front.jpg"
    };

    glGenTextures(1, &this->vboTexture);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, this->vboTexture);
    for (size_t i=0 ; i<skybox_images.size(); i++) {
        std::string image = skybox_images[i];

        int tWidth, tHeight, tChannels;
        unsigned char* tPixels = stbi_load(image.c_str(), &tWidth, &tHeight, &tChannels, 0);
        if (!tPixels)
            Settings::Instance()->funcDoLog("Can't load Skybox texture image - " + image + " with error - " + std::string(stbi_failure_reason()));
        else {
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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tPixels);
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

    return true;
}

#pragma mark - Render

void Skybox::render(glm::mat4 matrixView, float plane_close, float plane_far, float fov) {
    if (this->glVAO > 0) {
        glBindVertexArray(this->glVAO);
        glUseProgram(this->shaderProgram);

        GLint currentDepthFuncMode;
        glGetIntegerv(GL_DEPTH_FUNC, &currentDepthFuncMode);

        glDepthFunc(GL_LEQUAL);

        glm::mat4 matrixProjection = glm::perspective(fov, (float)Settings::Instance()->SDL_Window_Width / (float)Settings::Instance()->SDL_Window_Height, plane_close, plane_far);
        glm::mat4 matrixMV = matrixProjection * matrixView;
        glUniformMatrix4fv(this->glVS_MVPMatrix, 1, GL_FALSE, glm::value_ptr(matrixMV));

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->vboTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUseProgram(0);
        glBindVertexArray(0);

        glDepthFunc(currentDepthFuncMode);
    }
}

#pragma mark - Utilities

std::string Skybox::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        Settings::Instance()->funcDoLog("Could not read file " + std::string(filePath) + ". File does not exist.");
        return "";
    }
    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}
