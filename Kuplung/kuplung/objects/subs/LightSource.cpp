//
//  LightSource.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "LightSource.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

void LightSource::destroy() {
    delete this->eyeSettings;

    delete this->positionX;
    delete this->positionY;
    delete this->positionZ;

    delete this->directionX;
    delete this->directionY;
    delete this->directionZ;

    delete this->scaleX;
    delete this->scaleY;
    delete this->scaleZ;

    delete this->rotateX;
    delete this->rotateY;
    delete this->rotateZ;

    delete this->ambient;
    delete this->diffuse;
    delete this->specular;

    this->meshLight->destroy();
    this->meshLightRay->destroy();
}

LightSource::LightSource(std::function<void(std::string)> doLog, objScene obj, LightSourceType type, std::string title, std::string description) {
    this->funcLog = doLog;

    this->type = type;
    this->title = title;
    this->description = description;
    this->initProperties();

    this->meshLight = new Light();
    this->meshLight->init(std::bind(&LightSource::logMessage, this, std::placeholders::_1), "light", Settings::Instance()->OpenGL_GLSL_Version);
    this->meshLight->setModel(obj.models[0].faces[0]);
    this->meshLight->initShaderProgram();
    this->meshLight->initBuffers(std::string(Settings::Instance()->appFolder()));

    this->meshLightRay = new LightRay();
    this->meshLightRay->init(std::bind(&LightSource::logMessage, this, std::placeholders::_1), "dots", Settings::Instance()->OpenGL_GLSL_Version);
    this->meshLightRay->initShaderProgram();
}

void LightSource::initProperties() {
    this->eyeSettings = new ObjectEye();
    this->eyeSettings->View_Eye = glm::vec3(1.0, 1.0, 1.0);
    this->eyeSettings->View_Center = glm::vec3(0.0, 0.0, 0.0);
    this->eyeSettings->View_Up = glm::vec3(0.0, 1.0, 0.0);

    this->positionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->positionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 5.0 });

    this->directionX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->directionY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ -2.0 });
    this->directionZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->scaleX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });
    this->scaleZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 1.0 });

    this->rotateX = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateY = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });
    this->rotateZ = new ObjectCoordinate({ /*.animate=*/ false, /*.point=*/ 0.0 });

    this->ambient = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 0.3, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->diffuse = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 1.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });
    this->specular = new MaterialColor({ /*.colorPickerOpen=*/ false, /*.animate=*/ false, /*.strength=*/ 0.0, /*.color=*/ glm::vec3(1.0, 1.0, 1.0) });

    this->matrixModel = glm::mat4(1.0);
}

void LightSource::render(glm::mat4 mtxProjection, glm::mat4 mtxCamera, glm::mat4 mtxGrid, bool fixedGridWorld) {
    this->matrixProjection = mtxProjection;
    this->matrixCamera = mtxCamera;

    this->matrixModel = glm::mat4(1.0);
    if (fixedGridWorld)
        this->matrixModel = mtxGrid;
    this->matrixModel = glm::scale(this->matrixModel, glm::vec3(this->scaleX->point, this->scaleY->point, this->scaleZ->point));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateX->point), glm::vec3(1, 0, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateY->point), glm::vec3(0, 1, 0));
    this->matrixModel = glm::rotate(this->matrixModel, glm::radians(this->rotateZ->point), glm::vec3(0, 0, 1));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(0, 0, 0));
    this->matrixModel = glm::translate(this->matrixModel, glm::vec3(this->positionX->point, this->positionY->point, this->positionZ->point));

    glm::vec3 vLightDirection = glm::vec3(this->directionX->point, this->directionY->point, this->directionZ->point);

    // lamp
    this->meshLight->render(this->matrixProjection, this->matrixCamera, this->matrixModel);

    // direction line
    glm::mat4 mtxModelDot = this->matrixModel;
    mtxModelDot = glm::rotate(mtxModelDot, glm::radians(90.0f), glm::vec3(1, 0, 0));
    this->meshLightRay->initBuffers(glm::vec3(0, 0, 0), vLightDirection, true);
    this->meshLightRay->render(this->matrixProjection, this->matrixCamera, mtxModelDot);
}

void LightSource::logMessage(std::string message) {
    this->funcLog("[LightSource] " + message);
}
