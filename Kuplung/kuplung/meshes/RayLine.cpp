//
//  RayLine.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RayLine.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

RayLine::~RayLine() {
    this->destroy();
}

void RayLine::destroy() {
    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboIndices);
    glDeleteBuffers(1, &this->vboColors);

    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void RayLine::init() {
    this->glUtils = new GLUtils();
    this->initProperties();
}

void RayLine::initProperties() {
    this->matrixModel = glm::mat4(1.0);
}

#pragma mark - Public

bool RayLine::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/ray_line.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/ray_line.frag";
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
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_color");
    }

    return success;
}

void RayLine::initBuffers(glm::vec3 vecFrom, glm::vec3 vecTo) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->dataVertices = {
        vecFrom.x, vecFrom.y, vecFrom.z,
        vecTo.x, vecTo.y, vecTo.z
    };

    this->dataColors = {
        0.0, 0.0, 0.0,
        0.0, 0.0, 0.0
    };

    this->dataIndices = {
        0, 1, 2,
        3, 4, 5
    };

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->dataVertices.size() * sizeof(GLfloat), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, this->dataColors.size() * sizeof(GLfloat), &this->dataColors[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glUniformColor);
    glVertexAttribPointer(this->glUniformColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void RayLine::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = matrixProjection;
        this->matrixCamera = matrixCamera;

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);
        glLineWidth((GLfloat)2.5f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_LINE_STRIP, 0, 2);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string RayLine::readFile(const char *filePath) {
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
