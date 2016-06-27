//
//  Spaceship.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/22/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Spaceship.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STBI_FAILURE_USERMSG
#include "kuplung/utilities/stb/stb_image.h"

#pragma mark - Destroy

Spaceship::~Spaceship() {
    this->destroy();
}

void Spaceship::destroy() {
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
}

#pragma mark - Initialization

void Spaceship::init() {
    this->glUtils = new GLUtils();
    this->spaceshipGenerator = new SpaceshipMeshGenerator();

    this->Setting_UseTexture = false;
    this->Setting_Wireframe = false;
}

#pragma mark - Public

bool Spaceship::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/spaceship.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/spaceship.frag";
    std::string shaderSourceFragment = readFile(shaderPath.c_str());
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
        this->glAttributeVertexNormal = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexNormal");
        this->glAttributeTextureCoord = this->glUtils->glGetAttribute(this->shaderProgram, "a_textureCoord");
        this->glUniformHasTexture = this->glUtils->glGetUniform(this->shaderProgram, "has_texture");
        this->glUniformSamplerTexture = this->glUtils->glGetUniform(this->shaderProgram, "sampler_texture");
        this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "a_color");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void Spaceship::initBuffers(int gridSize) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->spaceshipGenerator->generate(gridSize);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->vertices.size() * sizeof(glm::vec3), &this->spaceshipGenerator->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // normals
    glGenBuffers(1, &this->vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->normals.size() * sizeof(glm::vec3), &this->spaceshipGenerator->normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexNormal);
    glVertexAttribPointer(this->glAttributeVertexNormal, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // texture
    glGenBuffers(1, &this->vboTextureCoordinates);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboTextureCoordinates);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->uvs.size() * sizeof(glm::vec2), &this->spaceshipGenerator->uvs[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeTextureCoord);
    glVertexAttribPointer(this->glAttributeTextureCoord, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, this->spaceshipGenerator->colors.size() * sizeof(glm::vec3), &this->spaceshipGenerator->colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeColor);
    glVertexAttribPointer(this->glAttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->spaceshipGenerator->indices.size() * sizeof(GLuint), &this->spaceshipGenerator->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void Spaceship::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);

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

        glDrawElements(GL_TRIANGLES, int(this->spaceshipGenerator->indices.size()), GL_UNSIGNED_INT, nullptr);

        if (this->Setting_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string Spaceship::readFile(const char *filePath) {
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
