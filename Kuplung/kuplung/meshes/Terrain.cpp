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

#pragma mark - Destroy

Terrain::~Terrain() {
    this->destroy();
}

void Terrain::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void Terrain::init() {
    this->glUtils = new GLUtils();
    this->terrainGenerator = new HeightmapGenerator();
}

#pragma mark - Public

bool Terrain::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/terrain.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/terrain.frag";
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
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "a_color");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void Terrain::initBuffers(std::string assetsFolder) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->terrainGenerator->generateTerrain(assetsFolder, 0, 0);
    this->heightmapImage = this->terrainGenerator->heightmapImage;

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->terrainGenerator->vertices.size() * sizeof(GLfloat), &this->terrainGenerator->vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, this->terrainGenerator->colors.size() * sizeof(GLfloat), &this->terrainGenerator->colors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeColor);
    glVertexAttribPointer(this->glAttributeColor, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->terrainGenerator->indices.size() * sizeof(GLuint), &this->terrainGenerator->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void Terrain::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        //glEnable(GL_CULL_FACE);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);
        glDrawElements(GL_TRIANGLES, (int)this->terrainGenerator->indices.size(), GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_LINES, 0, (int)this->terrainGenerator->vertices.size());
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string Terrain::readFile(const char *filePath) {
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
