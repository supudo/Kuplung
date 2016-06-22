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

void WorldGrid::init() {
    this->glUtils = new GLUtils();
    this->initProperties(10);
}

void WorldGrid::initProperties(int size) {
    this->showGrid = true;
    this->actAsMirror = false;
    this->actAsMirrorNeedsChange = true;
    this->transparency = 0.5;

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0f });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -90.0f });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->matrixModel = glm::mat4(1.0);

    this->mirrorSurface = new MirrorSurface();

    this->mirrorSurface->translateX = 0.0;
    this->mirrorSurface->translateY = 5.0;
    this->mirrorSurface->translateZ = -5.0;
    this->mirrorSurface->rotateX = 90.0f;
    this->mirrorSurface->rotateY = 0.0f;
    this->mirrorSurface->rotateZ = 0.0f;
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
        this->glAttributeActAsMirror = this->glUtils->glGetUniform(this->shaderProgram, "a_actAsMirror");
        this->glAttributeAlpha = this->glUtils->glGetUniform(this->shaderProgram, "a_alpha");
        this->glUniformMVPMatrix = this->glUtils->glGetUniform(this->shaderProgram, "u_MVPMatrix");
        this->glAttributeColor = this->glUtils->glGetAttribute(this->shaderProgram, "fs_color");
    }

    return success;
}

void WorldGrid::initBuffers(int gridSize, float unitSize) {
    glGenVertexArrays(1, &this->glVAO);
    glBindVertexArray(this->glVAO);

    if (!this->actAsMirror) {
        this->actAsMirrorNeedsChange = true;
        this->gridSize = gridSize;
        this->gridSizeVertex = this->gridSize;

        if (this->gridSizeVertex % 2 == 0)
            this->gridSizeVertex += 1;

        this->dataVertices.clear();
        this->dataTexCoords.clear();
        this->dataNormals.clear();
        this->dataColors.clear();
        this->dataIndices.clear();

        int gridMinus = this->gridSizeVertex / 2;
        std::vector<GridMeshPoint3D> verticesData;
        bool h;
        for (int i = 0; i < (this->gridSizeVertex * 2); i++) {
            for (int j = 0; j < this->gridSizeVertex; j++) {
                h = true;
                if (i >= this->gridSizeVertex)
                    h = false;
                if (h) {
                    GridMeshPoint3D p;
                    p.x = (j - gridMinus) * unitSize;
                    p.y = (i - gridMinus) * unitSize;
                    p.z = 0;
                    verticesData.push_back(p);
                    if (p.y == 0) {
                        this->dataColors.push_back(1.0f);
                        this->dataColors.push_back(0.0f);
                        this->dataColors.push_back(0.0f);
                    }
                    else {
                        this->dataColors.push_back(0.7f);
                        this->dataColors.push_back(0.7f);
                        this->dataColors.push_back(0.7f);
                    }
                }
                else {
                    GridMeshPoint3D p;
                    p.x = (i - this->gridSizeVertex - gridMinus) * unitSize;
                    p.y = (j - gridMinus) * unitSize;
                    p.z = 0;
                    verticesData.push_back(p);
                    if (p.x == 0) {
                        this->dataColors.push_back(0.0f);
                        this->dataColors.push_back(1.0f);
                        this->dataColors.push_back(0.0f);
                    }
                    else {
                        this->dataColors.push_back(0.7f);
                        this->dataColors.push_back(0.7f);
                        this->dataColors.push_back(0.7f);
                    }
                }
            }
        }

        this->zIndex = (int)verticesData.size();

        GridMeshPoint3D p_z_0;
        p_z_0.x = 0.0f;
        p_z_0.y = 0.0f;
        p_z_0.z = -1.0f * (float)gridMinus;
        verticesData.push_back(p_z_0);
        this->dataColors.push_back(0.0f);
        this->dataColors.push_back(0.0f);
        this->dataColors.push_back(1.0f);

        GridMeshPoint3D p_z_1;
        p_z_1.x = 0.0f;
        p_z_1.y = 0.0f;
        p_z_1.z = (float)gridMinus;
        verticesData.push_back(p_z_1);
        this->dataColors.push_back(0.0f);
        this->dataColors.push_back(0.0f);
        this->dataColors.push_back(1.0f);

        // vertices
        glGenBuffers(1, &this->vboVertices);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices);
        glBufferData(GL_ARRAY_BUFFER, verticesData.size() * sizeof(GridMeshPoint3D) * sizeof(GLfloat), &verticesData[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeVertexPosition);
        glVertexAttribPointer(this->glAttributeVertexPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // colors
        glGenBuffers(1, &this->vboColors);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboColors);
        glBufferData(GL_ARRAY_BUFFER, this->dataColors.size() * sizeof(GLfloat), &this->dataColors[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(this->glAttributeColor);
        glVertexAttribPointer(this->glAttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // indices
        glGenBuffers(1, &this->vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);
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

        this->dataTexCoords = {
            0.0f,  1.0f,
            1.0f,  1.0f,
            1.0f,  0.0f,
            0.0f,  0.0f
        };

        this->dataNormals = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
        };

        this->dataColors = {
            0.7f, 0.7f, 0.7f,
            0.7f, 0.7f, 0.7f,
            0.7f, 0.7f, 0.7f,
            0.7f, 0.7f, 0.7f,
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
        glEnableVertexAttribArray(this->glAttributeColor);
        glVertexAttribPointer(this->glAttributeColor, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);

        // indices
        glGenBuffers(1, &this->vboIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->dataIndices.size() * sizeof(GLuint), &this->dataIndices[0], GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
}

#pragma mark - Render

void WorldGrid::render(glm::mat4 matrixProjection, glm::mat4 matrixCamera, bool showZAxis) {
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

        glm::mat4 mvpMatrix = matrixProjection * matrixCamera * this->matrixModel;
        glUniformMatrix4fv(this->glUniformMVPMatrix, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

        // draw
        glBindVertexArray(this->glVAO);

        if (this->actAsMirror && this->actAsMirrorNeedsChange)
            this->initBuffers(this->gridSize, 1.0);
        else if (!this->actAsMirror && !this->actAsMirrorNeedsChange)
            this->initBuffers(this->gridSize, 1.0);

        if (!this->actAsMirror) {
            glFrontFace(GL_CCW);
            glCullFace(GL_BACK);
            glLineWidth((GLfloat)2.5f);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glUniform1f(this->glAttributeAlpha, 1.0);
            glUniform1i(this->glAttributeActAsMirror, 0);

            for (int i = 0; i < this->gridSizeVertex * 2; i++)
                glDrawArrays(GL_LINE_STRIP, this->gridSizeVertex * i, this->gridSizeVertex);
            for (int i = 0; i < this->gridSizeVertex; i++)
                glDrawArrays(GL_LINE_STRIP, 0, this->gridSizeVertex);
            if (showZAxis)
                glDrawArrays(GL_LINES, this->zIndex, 2);
        }
        else {
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

        glBindVertexArray(0);

        glUseProgram(0);
    }
}

#pragma mark - Utilities

std::string WorldGrid::readFile(const char *filePath) {
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
