//
//  CoordinateSystem.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "CoordinateSystem.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

CoordinateSystem::~CoordinateSystem() {
    this->destroy();
}

void CoordinateSystem::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void CoordinateSystem::init(std::function<void(std::string)> doLog, std::string shaderName, int glslVersion) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&CoordinateSystem::doLog, this, std::placeholders::_1));
    this->shaderName = shaderName;
    this->glslVersion = glslVersion;
}

#pragma mark - Public

bool CoordinateSystem::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".vert";
    std::string shaderVertexSource = readFile(shaderPath.c_str());
    shaderVertexSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderVertexSource;
    const char *shader_vertex = shaderVertexSource.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/" + this->shaderName + ".frag";
    std::string shaderFragmentSource = readFile(shaderPath.c_str());
    shaderFragmentSource = "#version " + std::to_string(this->glslVersion) + "\n" + shaderFragmentSource;
    const char *shader_fragment = shaderFragmentSource.c_str();

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
        this->doLog(Settings::Instance()->string_format("Error linking program %d!\n", this->shaderProgram));
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "a_axisColor");
    }

    return success;
}

void CoordinateSystem::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->axisSize = 6;
    GLfloat g_vertex_buffer_data[] = {
        // X
        -100, 0, 0,
        100, 0, 0,

        // Y
        0, -100, 0,
        0, 100, 0,

        // Z
        0, 0, -100,
        0, 0, 100
    };

    GLfloat g_color_buffer_data[] = {
        // X - red
        1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,

        // Y - green
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,

        // Z - blue
        0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data) * sizeof(GLfloat), g_color_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeColor);
    glVertexAttribPointer(this->glAttributeColor, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    glBindVertexArray(0);
}

#pragma mark - Render

void CoordinateSystem::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);
        glLineWidth((GLfloat)5.5f);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);
        glDrawArrays(GL_LINES, 0, this->axisSize);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string CoordinateSystem::readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        this->doLog("Could not read file " + std::string(filePath) + ". File does not exist.");
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

void CoordinateSystem::doLog(std::string logMessage) {
    this->doLogFunc("[CoordinateSystem] " + logMessage);
}
