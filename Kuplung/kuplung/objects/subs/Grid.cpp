//
//  Grid.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Grid.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

void Grid::destroy() {
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

    this->sceneGridHorizontal->destroy();
    this->sceneGridVertical->destroy();
}

Grid::Grid(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description) {
    this->funcLog = doLog;
}

void Grid::initProperties(int size) {
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

    this->sceneGridHorizontal = new WorldGrid();
    this->sceneGridHorizontal->init(std::bind(&Grid::logMessage, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
    this->sceneGridHorizontal->initShaderProgram();
    this->sceneGridHorizontal->initBuffers(size, true, 1);

    this->sceneGridVertical = new WorldGrid();
    this->sceneGridVertical->init(std::bind(&Grid::logMessage, this, std::placeholders::_1), "grid", Settings::Instance()->OpenGL_GLSL_Version);
    this->sceneGridVertical->initShaderProgram();
    this->sceneGridVertical->initBuffers(size, false, 1);
}

void Grid::render(glm::mat4 mtxProjection, glm::mat4 mtxCamera) {
    this->matrixProjection = mtxProjection;
    this->matrixCamera = mtxCamera;

    this->matrixModel = glm::mat4(1.0);
    this->matrixModel = glm::scale(this->matrixModel, glm::vec3(this->scaleX->point, this->scaleY->point, this->scaleZ->point));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

    if (Settings::Instance()->showGrid) {
        this->sceneGridHorizontal->render(this->matrixProjection, this->matrixCamera, this->matrixModel);
        this->sceneGridVertical->render(this->matrixProjection, this->matrixCamera, this->matrixModel);
    }
}

void Grid::logMessage(std::string message) {
    this->funcLog("[Grid] " + message);
}
