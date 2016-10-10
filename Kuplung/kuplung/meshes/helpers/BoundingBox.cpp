//
//  BoundingBox.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "BoundingBox.hpp"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

BoundingBox::BoundingBox() {
}

BoundingBox::~BoundingBox() {
    this->destroy();
}

void BoundingBox::destroy() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboIndices);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

#pragma mark - Public

bool BoundingBox::initShaderProgram() {
    this->glUtils = std::make_unique<GLUtils>();

    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/bounding_box.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/bounding_box.frag";
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

void BoundingBox::initBuffers(MeshModel meshModel) {
    this->meshModel = meshModel;

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    // Cube 1x1x1, centered on origin

    // vertices
    this->dataVertices = {
        -0.5, -0.5, -0.5, 1.0,
         0.5, -0.5, -0.5, 1.0,
         0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
         0.5, -0.5,  0.5, 1.0,
         0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0
    };
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, this->dataVertices.size() * sizeof(GLfloat), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), NULL);

    // indices
    this->dataIndices = {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

    this->min_x = this->max_x = this->meshModel.vertices[0].x;
    this->min_y = this->max_y = this->meshModel.vertices[0].y;
    this->min_z = this->max_z = this->meshModel.vertices[0].z;
    for (unsigned int i = 0; i < this->meshModel.vertices.size(); i++) {
        if (this->meshModel.vertices[i].x < this->min_x) this->min_x = this->meshModel.vertices[i].x;
        if (this->meshModel.vertices[i].x > this->max_x) this->max_x = this->meshModel.vertices[i].x;
        if (this->meshModel.vertices[i].y < this->min_y) this->min_y = this->meshModel.vertices[i].y;
        if (this->meshModel.vertices[i].y > this->max_y) this->max_y = this->meshModel.vertices[i].y;
        if (this->meshModel.vertices[i].z < this->min_z) this->min_z = this->meshModel.vertices[i].z;
        if (this->meshModel.vertices[i].z > this->max_z) this->max_z = this->meshModel.vertices[i].z;
    }

    float padding = Settings::Instance()->BoundingBoxPadding;
    this->min_x = (this->min_x > 0) ? this->min_x + padding : this->min_x - padding;
    this->max_x = (this->max_x > 0) ? this->max_x + padding : this->max_x - padding;
    this->min_y = (this->min_y > 0) ? this->min_y + padding : this->min_y - padding;
    this->max_y = (this->max_y > 0) ? this->max_y + padding : this->max_y - padding;
    this->min_z = (this->min_z > 0) ? this->min_z + padding : this->min_z - padding;
    this->max_z = (this->max_z > 0) ? this->max_z + padding : this->max_z - padding;

    this->size = glm::vec3(this->max_x - this->min_x, this->max_y - this->min_y, this->max_z - this->min_z);
    this->center = glm::vec3((this->min_x + this->max_x) / 2, (this->min_y + this->max_y) / 2, (this->min_z + this->max_z) / 2) * .5f;
    this->matrixTransform = glm::scale(glm::mat4(1), this->size) * glm::translate(glm::mat4(1), this->center);

    glBindVertexArray(0);

    Settings::Instance()->BoundingBoxRefresh = false;
}

#pragma mark - Render

void BoundingBox::render(glm::mat4 matrixMVP, glm::vec4 outlineColor) {
    if (Settings::Instance()->BoundingBoxRefresh)
        this->initBuffers(this->meshModel);
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glm::mat4 mtxModel = matrixMVP * this->matrixTransform;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtxModel));

        glUniform3f(this->glUniformColor, outlineColor.r, outlineColor.g, outlineColor.b);

        glBindVertexArray(this->glVAO);

        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(GLuint)));
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(GLuint)));

        glBindVertexArray(0);

        glUseProgram(0);
    }
}
