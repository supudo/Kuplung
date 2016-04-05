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

    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;

    delete this->rotateCenterX;
    delete this->rotateCenterY;
    delete this->rotateCenterZ;
}

void Camera::initProperties() {
    this->eyeSettings = new ObjectEye();
    this->eyeSettings->View_Eye = glm::vec3(0.0, 0.0, 3.0);
    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->eyeSettings->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -10.0f });

//    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -71.0f });
//    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -36.0f });
//    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 10.0f });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 330.0f });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

    this->rotateCenterX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateCenterY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });
    this->rotateCenterZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0f });

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

glm::vec3 Camera::createRay(float mouse_x, float mouse_y, float fov, float ratio, float near, float far) {
    float mouseX = mouse_x / (Settings::Instance()->SDL_Window_Width * 0.5f) - 1.0f;
    float mouseY = mouse_y / (Settings::Instance()->SDL_Window_Height * 0.5f) - 1.0f;

    glm::mat4 proj = glm::perspective(fov, ratio, near, far);

    glm::vec3 cameraDirection = glm::vec3();
    glm::vec3 cameraUpVector = glm::vec3(0, 0, 1);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f), cameraDirection, cameraUpVector);

    glm::mat4 invVP = glm::inverse(proj * view);
    glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
    glm::vec4 worldPos = invVP * screenPos;

    return glm::normalize(glm::vec3(worldPos));
}

PixelDataPoint Camera::getClickData(int x, int y, int height) {
    PixelDataPoint p;
    glReadPixels(x, height - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, p.color);
    glReadPixels(x, height - y - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &p.depth);
    glReadPixels(x, height - y - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_INT, &p.index);
    return p;
}
