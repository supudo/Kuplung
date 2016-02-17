//
//  Camera.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

void Camera::destroy() {
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
}

Camera::Camera(std::function<void(std::string)> doLog, objScene obj, std::string title, std::string description) {
    this->funcLog = doLog;
}

void Camera::initProperties() {
    this->eyeSettings = new ObjectEye();
    this->eyeSettings->View_Eye = glm::vec3(0.0, 0.0, 3.0);
    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->eyeSettings->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -10.0 });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -71.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -36.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->matrixCamera = glm::mat4(1.0);
}

void Camera::render(float Setting_PlaneClose, float Setting_PlaneFar) {
    this->matrixCamera = glm::lookAt(this->eyeSettings->View_Eye, this->eyeSettings->View_Center, this->eyeSettings->View_Up);

    //this->matrixCamera = glm::ortho(0.0f, (float)Settings::Instance()->SDL_Window_Width, 0.0f, (float)Settings::Instance()->SDL_Window_Height, Setting_PlaneClose, Setting_PlaneFar);

    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));
    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));

    this->cameraPosition = glm::vec3(this->matrixCamera[3].x, this->matrixCamera[3].y, this->matrixCamera[3].z);
}

void Camera::logMessage(std::string message) {
    this->funcLog("[Camera] " + message);
}
