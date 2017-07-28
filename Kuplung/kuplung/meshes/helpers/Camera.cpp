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
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

Camera::Camera() {
}

Camera::~Camera() {
    this->eyeSettings.reset();

    this->positionX.reset();
    this->positionY.reset();
    this->positionZ.reset();

    this->rotateX.reset();
    this->rotateY.reset();
    this->rotateZ.reset();

    this->rotateCenterX.reset();
    this->rotateCenterY.reset();
    this->rotateCenterZ.reset();
}

void Camera::initProperties() {
    this->eyeSettings = std::make_unique<ObjectEye>();
//    this->eyeSettings->View_Eye = glm::vec3(0.0, 0.0, 3.0);
//    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
//    this->eyeSettings->View_Up = glm::vec3(0.0, FLT_EPSILON, 1.0); // TODO: doesn't work with 0.0, can't figure why

//    this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
//    this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f);
//    this->positionZ = std::make_unique<ObjectCoordinate>(false, -10.0f);

//    this->rotateX = std::make_unique<ObjectCoordinate>(false, 30.0f);
//    this->rotateY = std::make_unique<ObjectCoordinate>(false, 334.0f);
//    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

//    this->rotateCenterX = std::make_unique<ObjectCoordinate>(false, 0.0f);
//    this->rotateCenterY = std::make_unique<ObjectCoordinate>(false, 0.0f);
//    this->rotateCenterZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->eyeSettings->View_Eye = glm::vec3(0, 0, 10);
    this->eyeSettings->View_Center = glm::vec3(0, 0, 0);
    this->eyeSettings->View_Up = glm::vec3(0, -1, 0);

//    this->eyeSettings->View_Eye = glm::vec3(0, 2, 0);
//    this->eyeSettings->View_Center = glm::vec3(0, 0, -4);
//    this->eyeSettings->View_Up = glm::vec3(0, 1, 0);

    this->positionX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->positionY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->positionZ = std::make_unique<ObjectCoordinate>(false, -16.0f);

    this->rotateX = std::make_unique<ObjectCoordinate>(false, 160.0f);
    this->rotateY = std::make_unique<ObjectCoordinate>(false, 140.0f);
    this->rotateZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->rotateCenterX = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateCenterY = std::make_unique<ObjectCoordinate>(false, 0.0f);
    this->rotateCenterZ = std::make_unique<ObjectCoordinate>(false, 0.0f);

    this->matrixCamera = glm::mat4(1.0);
}

void Camera::render() {
    this->matrixCamera = glm::lookAt(this->eyeSettings->View_Eye, this->eyeSettings->View_Center, this->eyeSettings->View_Up);

    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
    this->matrixCamera = glm::translate(this->matrixCamera, glm::vec3(0, 0, 0));

    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterX->point), glm::vec3(1, 0, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterY->point), glm::vec3(0, 1, 0));
    this->matrixCamera = glm::rotate(this->matrixCamera, glm::radians(this->rotateCenterZ->point), glm::vec3(0, 0, 1));

    this->cameraPosition = glm::vec3(this->matrixCamera[3].x, this->matrixCamera[3].y, this->matrixCamera[3].z);
}

glm::vec3 Camera::createRay(const float mouse_x, const float mouse_y, const float fov, const float ratio, const float pNear, const float pFar) {
    float mouseX = mouse_x / (Settings::Instance()->SDL_Window_Width * 0.5f) - 1.0f;
    float mouseY = mouse_y / (Settings::Instance()->SDL_Window_Height * 0.5f) - 1.0f;

    glm::mat4 proj = glm::perspective(fov, ratio, pNear, pFar);

    glm::vec3 cameraDirection = glm::vec3(this->matrixCamera[2].x, this->matrixCamera[2].y, this->matrixCamera[2].z);
    glm::vec3 cameraUpVector = this->eyeSettings->View_Up;
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f), cameraDirection, cameraUpVector);

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
    glm::vec4 worldPos = invVP * screenPos;

    return glm::normalize(glm::vec3(worldPos));
}

PixelDataPoint Camera::getClickData(const int x, const int y, const int height) {
    PixelDataPoint p;
    glReadPixels(x, height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, p.color);
    glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &p.depth);
    glReadPixels(x, height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &p.index);
    return p;
}
