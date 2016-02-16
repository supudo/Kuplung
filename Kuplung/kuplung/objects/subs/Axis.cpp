//
//  Axis.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Axis.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

void Axis::destroy() {
    delete this->eyeSettings;

    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;

    this->axisSystem->destroy();
}

Axis::Axis(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description) {
    this->funcLog = doLog;
    this->initProperties();
}

void Axis::initProperties() {
    this->eyeSettings = new ObjectEye();
    this->eyeSettings->View_Eye = glm::vec3(1.0, 1.0, 1.0);
    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->eyeSettings->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->matrixModel = glm::mat4(1.0);

    this->axisSystem = new CoordinateSystem();
    this->axisSystem->init(std::bind(&Axis::logMessage, this, std::placeholders::_1), "axis", Settings::Instance()->OpenGL_GLSL_Version);
    this->axisSystem->initShaderProgram();
    this->axisSystem->initBuffers();
}

void Axis::render(glm::mat4 mtxProjection, glm::mat4 mtxCamera) {
    this->matrixProjection = mtxProjection;
    this->matrixCamera = mtxCamera;

    if (Settings::Instance()->showAxes) {
        float axisW = 120;
        float axisH = (Settings::Instance()->SDL_Window_Height * axisW) / Settings::Instance()->SDL_Window_Width;

        float axisX = 10;
        float axisY = 10;

        glViewport(axisX, axisY, axisW, axisH);

        this->matrixModel = glm::mat4(1.0);
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
        this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
        this->axisSystem->render(this->matrixProjection, this->matrixCamera, this->matrixModel);

        glViewport(0, 0, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    }
}

void Axis::logMessage(std::string message) {
    this->funcLog("[Axis] " + message);
}
