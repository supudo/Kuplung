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

VertexSphere::VertexSphere() {
    this->isSphere = false;
    this->showWireframes = false;
    this->circleSegments = 0;
}

VertexSphere::~VertexSphere() {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboIndices);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Public

bool VertexSphere::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/vertex_sphere.vert";
    std::string shaderSourceVertex = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/vertex_sphere.frag";
    std::string shaderSourceFragment = Settings::Instance()->glUtils->readFile(shaderPath.c_str());
    const char *shader_fragment = shaderSourceFragment.c_str();

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
        Settings::Instance()->funcDoLog("[VertexSphere] Error linking program " + std::to_string(this->shaderProgram) + "!\n");
        Settings::Instance()->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glUniformMVPMatrix = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glUniformColor = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_color");
        this->glUniformInnerLightDirection = Settings::Instance()->glUtils->glGetUniform(this->shaderProgram, "fs_innerLightDirection");
    }

    this->isSphere = true;
    this->showWireframes = true;

    return success;
}

void VertexSphere::initBuffers(MeshModel const& meshModel, const int& circleSegments, const float& radius) {
    this->circleSegments = circleSegments;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    this->dataVertices.clear();
    this->dataNormals.clear();
    this->dataIndices.clear();

    if (!this->isSphere) {
        float theta = 2 * 3.1415926f / float(this->circleSegments);
        float c = cosf(theta);
        float s = sinf(theta);
        float t;

        float r = radius;
        float x = r;
        float y = 0;

        for (size_t i=0; i<meshModel.vertices.size(); i++) {
            glm::vec3 vertex = meshModel.vertices[i];
            for (int j=0; j<this->circleSegments; j++) {
                this->dataVertices.push_back(glm::vec3(x + vertex.x, y + vertex.y, vertex.z));
                this->dataIndices.push_back(static_cast<unsigned int>(i));

                t = x;
                x = c * x - s * y;
                y = s * t + c * y;
            }
        }

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->dataVertices.size() * 3), &this->dataVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // indices
        glGenBuffers(1, &this->vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices[0], GL_STATIC_DRAW);
    }
    else {
        const float rings = 1.0f / float(this->circleSegments - 1);
        const float sectors = 1.0f / float(this->circleSegments - 1);

        static const double pi = glm::pi<double>();
        static const double pi_2 = glm::half_pi<double>();
        float p_x, p_y, p_z;
        glm::vec3 position;
        glm::vec3 v1, v2, v3, normal;

        for (size_t i=0; i<meshModel.vertices.size(); i++) {
            for (int y=0; y<this->circleSegments; ++y) {
                for (int x=0; x<this->circleSegments; ++x) {
                    p_x = float(cos(2 * pi * x * sectors) * sin(pi * y * rings));
                    p_y = float(sin(-pi_2 + pi * y * rings));
                    p_z = float(sin(2 * pi * x * sectors) * sin(pi * y * rings));
                    position = glm::vec3(p_x, p_y, p_z) * radius;
                    position += meshModel.vertices[i];
                    this->dataVertices.push_back(position);
                    this->dataNormals.push_back(position);
                }
            }

            for (int y=0; y<(this->circleSegments - 1); ++y) {
                for (int x=0; x<(this->circleSegments - 1); ++x) {
                    int start = (y * this->circleSegments + x) + (i * (this->circleSegments * this->circleSegments));

                    this->dataIndices.push_back(start);
                    this->dataIndices.push_back(start + 1);
                    this->dataIndices.push_back(start + this->circleSegments);
                    v1 = this->dataVertices[start];
                    v2 = this->dataVertices[start + 1];
                    v3 = this->dataVertices[start + this->circleSegments];
                    normal = glm::cross(v2 - v1, v3 - v1);
                    this->dataNormals[start] = normal;
                    this->dataNormals[start + 1] = normal;
                    this->dataNormals[start + this->circleSegments] = normal;

                    this->dataIndices.push_back(start + 1);
                    this->dataIndices.push_back(start + 1 + this->circleSegments);
                    this->dataIndices.push_back(start + this->circleSegments);
                    v1 = this->dataVertices[start];
                    v2 = this->dataVertices[start + this->circleSegments];
                    v3 = this->dataVertices[start + 1 + this->circleSegments];
                    normal = glm::cross(v2 - v1, v3 - v1);
                    this->dataNormals[start] = normal;
                    this->dataNormals[start + this->circleSegments] = normal;
                    this->dataNormals[start + 1 + this->circleSegments] = normal;
                }
            }
        }

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(this->dataVertices.size() * sizeof(glm::vec3)), &this->dataVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // normals
        glGenBuffers(1, &this->vboNormals);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboNormals);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLint>(this->dataNormals.size() * sizeof(glm::vec3)), &this->dataNormals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // indices
        glGenBuffers(1, &this->vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLint>(this->dataIndices.size() * sizeof(GLuint)), &this->dataIndices[0], GL_STATIC_DRAW);

    }

    glBindVertexArray(0);
}

#pragma mark - Render

void VertexSphere::render(const glm::mat4& matrixMVP, const glm::vec4& color) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(matrixMVP));
        glUniform3f(this->glUniformColor, color.r, color.g, color.b);
        glUniform3f(this->glUniformInnerLightDirection, 1.0f, 0.55f, 0.206f);

        glBindVertexArray(this->glVAO);
        if (!this->isSphere) {
            for (int i = 0; i < this->circleSegments; i++)
                glDrawArrays(GL_LINE_LOOP, this->circleSegments * i, this->circleSegments);
        }
        else {
            if (this->showWireframes)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, int(this->dataIndices.size()), GL_UNSIGNED_INT, 0);
            if (this->showWireframes)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        glBindVertexArray(0);

        glUseProgram(0);
    }
}
