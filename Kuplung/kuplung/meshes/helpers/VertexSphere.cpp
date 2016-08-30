//
//  VertexSphere.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "VertexSphere.hpp"

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

VertexSphere::VertexSphere() {
}

VertexSphere::~VertexSphere() {
    this->destroy();
}

void VertexSphere::destroy() {
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

bool VertexSphere::initShaderProgram() {
    this->glUtils = std::make_unique<GLUtils>();

    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/vertex_sphere.vert";
    std::string shaderSourceVertex = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/vertex_sphere.frag";
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
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformColor = this->glUtils->glGetUniform(this->shaderProgram, "fs_color");
    }

    return success;
}


void VertexSphere::initBuffers(MeshModel meshModel, int circleSegments, float radius) {
    this->circleSegments = circleSegments;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    float theta = 2 * 3.1415926 / float(this->circleSegments);
    float c = cosf(theta);
    float s = sinf(theta);
    float t;

    float r = radius;
    float x = r;
    float y = 0;

    this->dataVertices.clear();
    this->dataIndices.clear();

    for (size_t i=0; i<meshModel.vertices.size(); i++) {
        glm::vec3 vertex = meshModel.vertices[i];
        for (int j=0; j<this->circleSegments; j++) {
            this->dataVertices.push_back(glm::vec3(x + vertex.x, y + vertex.y, vertex.z));
            this->dataIndices.push_back(i);

            t = x;
            x = c * x - s * y;
            y = s * t + c * y;
        }
    }

    // vertices
    glGenBuffers(1, &this->vboVertices);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
    glBufferData(GL_ARRAY_BUFFER, int(this->dataVertices.size()) * sizeof(glm::vec3), &this->dataVertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(this->glAttributeVertexPosition);
    glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

    // indices
    glGenBuffers(1, &this->vboIndices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, int(this->dataIndices.size()) * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

#pragma mark - Render

void VertexSphere::render(glm::mat4 matrixMVP, glm::vec4 color) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

//        glFrontFace(GL_CCW);
//        glCullFace(GL_BACK);

//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LESS);
//        glDisable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(matrixMVP));
        glUniform3f(this->glUniformColor, color.r, color.g, color.b);

        glBindVertexArray(this->glVAO);
        for (int i = 0; i < this->circleSegments; i++)
            glDrawArrays(GL_LINE_LOOP, this->circleSegments * i, this->circleSegments);
        glBindVertexArray(0);

        glUseProgram(0);
    }
}
