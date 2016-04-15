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
}

#pragma mark - Public

bool BoundingBox::initShaderProgram() {
    this->glUtils = new GLUtils();

    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/bounding_box.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/bounding_box.frag";
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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return success;
}

void BoundingBox::initBuffers(objModelFace oFace) {
    this->oFace = oFace;

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

    GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
    min_x = max_x = this->oFace.vectors_vertices[0].x;
    min_y = max_y = this->oFace.vectors_vertices[0].y;
    min_z = max_z = this->oFace.vectors_vertices[0].z;
    for (unsigned int i = 0; i < this->oFace.vectors_vertices.size(); i++) {
        if (this->oFace.vectors_vertices[i].x < min_x) min_x = this->oFace.vectors_vertices[i].x;
        if (this->oFace.vectors_vertices[i].x > max_x) max_x = this->oFace.vectors_vertices[i].x;
        if (this->oFace.vectors_vertices[i].y < min_y) min_y = this->oFace.vectors_vertices[i].y;
        if (this->oFace.vectors_vertices[i].y > max_y) max_y = this->oFace.vectors_vertices[i].y;
        if (this->oFace.vectors_vertices[i].z < min_z) min_z = this->oFace.vectors_vertices[i].z;
        if (this->oFace.vectors_vertices[i].z > max_z) max_z = this->oFace.vectors_vertices[i].z;
    }

    float padding = 0.01f;
    min_x = (min_x > 0) ? min_x + padding : min_x - padding;
    max_x = (max_x > 0) ? max_x + padding : max_x - padding;
    min_y = (min_y > 0) ? min_y + padding : min_y - padding;
    max_y = (max_y > 0) ? max_y + padding : max_y - padding;
    min_z = (min_z > 0) ? min_z + padding : min_z - padding;
    max_z = (max_z > 0) ? max_z + padding : max_z - padding;

    glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
    this->matrixTransform = glm::scale(glm::mat4(1), size) * glm::translate(glm::mat4(1), center);

    glBindVertexArray(0);
}

#pragma mark - Render

void BoundingBox::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, glm::mat4 matrixModel) {
    if (this->glVAO > 0) {
        glUseProgram(this->shaderProgram);

        glm::mat4 mtx = matrixProjection * matrixCamera * matrixModel * this->matrixTransform;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mtx));

        glUniform3f(this->glUniformColor, 0.0, 0.0, 0.0);

        glBindVertexArray(this->glVAO);
//        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
//        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLuint)));
//        glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLuint)));

        int sz = 4;

//        for (int i = 0; i < sz * 2; i++)
//            glDrawArrays(GL_LINE_STRIP, sz * i, sz);
//        for (int i = 0; i < sz; i++)
//            glDrawArrays(GL_LINES, 0, sz);

//        for (int i = 0; i < sz; i++)
//            glDrawArrays(GL_LINES, 0, sz);

        glDrawElements(GL_LINE_LOOP, (int)this->dataIndices.size(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string BoundingBox::readFile(const char *filePath) {
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
