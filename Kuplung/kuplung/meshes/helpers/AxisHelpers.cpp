//
//  AxisHelpers.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/14/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "AxisHelpers.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

void AxisHelpers::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

void AxisHelpers::init() {
    this->glUtils = new GLUtils();
    this->initProperties();
}

void AxisHelpers::setModel(MeshModel meshModel) {
    this->meshModel = meshModel;
}

void AxisHelpers::initProperties() {
    this->matrixCamera = glm::mat4(1.0);
    this->matrixModel = glm::mat4(1.0);
}

bool AxisHelpers::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/axis_helpers.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/axis_helpers.frag";
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
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_color");
    }

    return success;
}

void AxisHelpers::initBuffers() {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->meshModel.countVertices * sizeof(glm::vec3), &this->meshModel.vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->meshModel.countIndices * sizeof(GLuint), &this->meshModel.indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void AxisHelpers::render(Camera *camera, glm::mat4 mtxProjection, glm::mat4 mtxCamera, glm::mat4 mtxGrid, bool fixedGridWorld, glm::vec3 position) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = mtxProjection;
        this->matrixCamera = mtxCamera;

        this->matrixModel = glm::mat4(1.0);
        if (fixedGridWorld)
            this->matrixModel = mtxGrid;

        this->matrixModel = glm::translate(this->matrixModel, position);

        glm::mat4 mvpMatrix = this->matrixProjection * this->matrixCamera * this->matrixModel;

        // drawing options
        glCullFace(GL_FRONT);
        glFrontFace(GL_CCW);

        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        glUniform3f(this->glUniformColor, this->meshModel.ModelMaterial.DiffuseColor.r, this->meshModel.ModelMaterial.DiffuseColor.g, this->meshModel.ModelMaterial.DiffuseColor.b);

        // draw
        glBindVertexArray(this->glVAO);
        glDrawElements(GL_TRIANGLES, this->meshModel.countIndices, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string AxisHelpers::readFile(const char *filePath) {
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
