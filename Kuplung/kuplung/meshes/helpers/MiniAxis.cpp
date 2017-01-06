//
//  MiniAxis.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "MiniAxis.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

MiniAxis::~MiniAxis() {
    this->rotateX.reset();
    this->rotateY.reset();
    this->rotateZ.reset();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

MiniAxis::MiniAxis() {
    this->axisSize = 6;
    this->initProperties();
}

void MiniAxis::initProperties() {
    this->showAxis = true;

    this->rotateX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->matrixModel = glm::mat4(1.0);
}

#pragma mark - Public

bool MiniAxis::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/axis.vert";
    std::string shaderVertexSource = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderVertexSource.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/axis.frag";
    std::string shaderFragmentSource = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderFragmentSource.c_str();

    this->shaderProgram = glCreateProgram();

    bool shaderCompilation = true;
    shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderVertex, GL_VERTEX_SHADER, shader_vertex);
    shaderCompilation &= Settings::Instance()->glUtils->compileAndAttachShader(this->shaderProgram, this->shaderFragment, GL_FRAGMENT_SHADER, shader_fragment);

    if (!shaderCompilation)
        return false;

    glLinkProgram(this->shaderProgram);

    GLint programSuccess = GL_TRUE;
    glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &programSuccess);
    if (programSuccess != GL_TRUE) {
        Settings::Instance()->funcDoLog("[MiniAxis] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else
        this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");

    return success;
}

void MiniAxis::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

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
    glBufferData(GL_ARRAY_BUFFER, std::extent<decltype(g_vertex_buffer_data)>::value * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // colors
    glGenBuffers(1, &this->vboColors);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
    glBufferData(GL_ARRAY_BUFFER, std::extent<decltype(g_color_buffer_data)>::value * sizeof(GLfloat), g_color_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    glBindVertexArray(0);
}

#pragma mark - Render

void MiniAxis::render(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera) {
    if (this->glVAO > 0 && this->showAxis) {
        glUseProgram(this->shaderProgram);

        int axisW = 120, axisH = (Settings::Instance()->SDL_Window_Height * axisW) / Settings::Instance()->SDL_Window_Width;
        int axisX = 10, axisY = 10;

        glViewport(axisX, axisY, axisW, axisH);

        this->matrixModel = glm::mat4(1.0);
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));

        glLineWidth(5.5f);

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);
        glDrawArrays(GL_LINES, 0, this->axisSize);

        glBindVertexArray(0);

        glUseProgram(0);

        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    }
}
