//
//  WorldGrid.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/5/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "WorldGrid.hpp"
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma mark - Destroy

WorldGrid::~WorldGrid() {
    this->destroy();
}

void WorldGrid::destroy() {
    delete this->eyeSettings;

    delete this->positionX;
    delete this->positionY;
    delete this->positionZ;

    delete this->scaleX;
    delete this->scaleY;
    delete this->scaleZ;

    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;

    glDeleteBuffers(1, &this->vboVertices);
    glDeleteBuffers(1, &this->vboIndices);

    glDisableVertexAttribArray(this->glAttributeVertexPosition);

    glDetachShader(this->shaderProgram, this->shaderVertex);
    glDetachShader(this->shaderProgram, this->shaderFragment);
    glDeleteProgram(this->shaderProgram);

    glDeleteShader(this->shaderVertex);
    glDeleteShader(this->shaderFragment);

    glDeleteVertexArrays(1, &this->glVAO);
}

#pragma mark - Initialization

void WorldGrid::init(std::function<void(std::string)> doLog) {
    this->doLogFunc = doLog;
    this->glUtils = new GLUtils();
    this->glUtils->init(std::bind(&WorldGrid::doLog, this, std::placeholders::_1));
    this->initProperties(10);
}

void WorldGrid::initProperties(int size) {
    this->showGrid = true;
    this->actAsMirror = true;
    this->actAsMirrorNeedsChange = true;
    this->transparency = 0.5;

    this->eyeSettings = new ObjectEye();
    this->eyeSettings->View_Eye = glm::vec3(1.0, 1.0, 1.0);
    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->eyeSettings->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->matrixModel = glm::mat4(1.0);
}

#pragma mark - Public

bool WorldGrid::initShaderProgram() {
    bool success = true;

    std::string shaderPath = Settings::Instance()->appFolder() + "/shaders/grid.vert";
    std::string shaderSourceVertex = readFile(shaderPath.c_str());
    const char *shader_vertex = shaderSourceVertex.c_str();

    shaderPath = Settings::Instance()->appFolder() + "/shaders/grid.frag";
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
        this->doLog(Settings::Instance()->string_format("Error linking program %d!\n", this->shaderProgram));
        this->glUtils->printProgramLog(this->shaderProgram);
        return success = false;
    }
    else {
        this->glAttributeVertexPosition = this->glUtils->glGetAttribute(this->shaderProgram, "a_vertexPosition");
        this->glAttributeActAsMirror = this->glUtils->glGetUniform(this->shaderProgram, "a_actAsMirror");
        this->glAttributeAlpha = this->glUtils->glGetUniform(this->shaderProgram, "a_alpha");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
    }

    return success;
}

void WorldGrid::initBuffers(int gridSize, float unitSize) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    if (!this->actAsMirror) {
        this->actAsMirrorNeedsChange = true;
        this->gridSize = gridSize;

        float gridMinus = this->gridSize / 2;
        GridMeshPoint2D verticesData[this->gridSize * 2][this->gridSize];
        bool h;

        this->dataVertices.clear();
        this->dataIndices.clear();
        for (int i = 0; i < (this->gridSize * 2); i++) {
            for (int j = 0; j < this->gridSize; j++) {
                h = true;
                if (i >= this->gridSize)
                    h = false;
                if (h) {
                    verticesData[i][j].y = (i - gridMinus) * unitSize;
                    verticesData[i][j].x = (j - gridMinus) * unitSize;
                }
                else {
                    verticesData[i][j].x = (i - this->gridSize - gridMinus) * unitSize;
                    verticesData[i][j].y = (j - gridMinus) * unitSize;
                }
            }
        }

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verticesData) * sizeof(GLfloat), verticesData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeVertexPosition);
        glVertexAttribPointer(this->glAttributeVertexPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
    }
    else {
        this->actAsMirrorNeedsChange = false;

        float planePoint = (float)(this->gridSize / 2);

        this->dataVertices = {
            planePoint, planePoint, 0.0,
            planePoint, -1 * planePoint, 0.0,
            -1 * planePoint, -1 * planePoint, 0.0,
            -1 * planePoint, planePoint, 0.0,
            planePoint, planePoint, 0.0,
            -1 * planePoint, -1 * planePoint, 0.0
        };

        //GLuint indicesData[6] = {
        this->dataIndices = {
            0, 1, 2, 3, 4, 5
        };

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, this->dataVertices.size() * sizeof(GLfloat), &this->dataVertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeVertexPosition);
        glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // indices
        glGenBuffers(1, &this->vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

#pragma mark - Render

void WorldGrid::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera) {
    if (this->glVAO > 0 && this->showGrid) {
        glUseProgram(this->shaderProgram);

        this->matrixProjection = matrixProjection;
        this->matrixCamera = matrixCamera;

        this->matrixModel = glm::mat4(1.0);
        this->matrixModel = glm::scale(this->matrixModel, glm::vec3(this->scaleX->point, this->scaleY->point, this->scaleZ->point));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
        this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);

        if (this->actAsMirror && this->actAsMirrorNeedsChange)
            this->initBuffers(this->gridSize, 1.0);
        else if (!this->actAsMirror && !this->actAsMirrorNeedsChange)
            this->initBuffers(this->gridSize, 1.0);

        if (this->actAsMirror) {
            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);

            glUniform1f(this->glAttributeAlpha, this->transparency);
            glUniform1i(this->glAttributeActAsMirror, 1);

            glDepthMask(GL_FALSE);

            glDrawElements(GL_TRIANGLES, sizeof(this->dataIndices), GL_UNSIGNED_INT, nullptr);

            glDepthMask(GL_TRUE);

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glLineWidth((GLfloat)2.5f);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUniform1f(this->glAttributeAlpha, 1.0);
            glUniform1i(this->glAttributeActAsMirror, 0);

            for (int i = 0; i < this->gridSize * 2; i++)
                glDrawArrays(GL_LINE_STRIP, this->gridSize * i, this->gridSize);
            for (int i = 0; i < this->gridSize; i++)
                glDrawArrays(GL_LINE_STRIP, 0, this->gridSize);
        }

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string WorldGrid::readFile(const char *filePath) {
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

void WorldGrid::doLog(std::string logMessage) {
    this->doLogFunc("[WorldGrid] " + logMessage);
}
