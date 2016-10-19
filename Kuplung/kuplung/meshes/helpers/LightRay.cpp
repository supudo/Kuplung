//
//  LightRay.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "LightRay.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

LightRay::~LightRay() {
    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);

    this->glUtils.reset();
}

LightRay::LightRay() {
    this->glUtils = std::make_unique<GLUtils>();
    this->axisSize = -1;
    this->x = -1;
    this->y = -1;
    this->z = -1;
}

#pragma mark - Public

bool LightRay::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/light_ray.vert";
    std::string shaderVertexSource = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderVertexSource.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/light_ray.frag";
    std::string shaderFragmentSource = this->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderFragmentSource.c_str();

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
    }

    this->x = 999;
    this->y = 999;
    this->z = 999;
    glGenVertexArrays(1, &this->glVAO);

    return success;
}

void LightRay::initBuffers(glm::vec3 position, glm::vec3 direction, bool simple) {
    if (position.x != x && position.y != y && position.z != z) {
        this->x = position.x;
        this->y = position.y;
        this->z = position.z;

        glBindVertexArray(this->glVAO);

        this->axisSize = (simple ? 2 : 12);
        std::vector<GLfloat> vertices;
        if (simple) {
            vertices.push_back(position.x);
            vertices.push_back(position.y);
            vertices.push_back(position.z);

            vertices.push_back(direction.x);
            vertices.push_back(direction.y);
            vertices.push_back(direction.z);
        }
        else {
            GLfloat g_vertex_buffer_data[] = {
                 -0.866024971, 0, 0.5,
                 0.866024971, 0, -0.5,
                 0, 0, -1,
                 0, 0, -1,
                 -0.866024971, 0, -0.5,
                 -0.866024971, 0, 0.5,
                 -0.866024971, 0, 0.5,
                 0, 0, 1,
                 0.866024971, 0, 0.5,
                 0.866024971, 0, 0.5,
                 0.866024971, 0, -0.5,
                 -0.866024971, 0, 0.5
            };
            for (size_t i=0; i<sizeof(g_vertex_buffer_data); i++) {
                vertices.push_back(g_vertex_buffer_data[i]);
            }
        }

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeVertexPosition);
        glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        if (!simple) {
            std::vector<GLuint> indices;
            for (size_t i=0; i<12; i++) {
                indices.push_back((int)i);
            }

            // indices
            glGenBuffers(1, &this->vboIndices);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
        }

        glBindVertexArray(0);
    }
}

#pragma mark - Render

void LightRay::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
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
        if (this->axisSize > 2)
            glDrawElements(GL_TRIANGLES, this->axisSize, GL_UNSIGNED_INT, nullptr);
        else
            glDrawArrays(GL_LINES, 0, this->axisSize);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}
