//
//  RayPicking.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RayPicking.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/intersect.hpp>

void RayPicking::selectModel(std::vector<ModelFace*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject, std::string *selectedMaterialID) {
    this->meshModelFaces = meshModelFaces;

    this->sceneSelectedModelObject = *sceneSelectedModelObject;
    this->selectedMaterialID = *selectedMaterialID;

    this->pick3();

    *sceneSelectedModelObject = this->sceneSelectedModelObject;
    *selectedMaterialID = this->selectedMaterialID;
    *rayLines = this->rayLines;
}

void RayPicking::pick5() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    glm::vec4 lRayStart_NDC(
        ((float)mouse_x / (float)Settings::Instance()->SDL_Window_Width - 0.5f) * 2.0f,
        ((float)mouse_y / (float)Settings::Instance()->SDL_Window_Height - 0.5f) * 2.0f,
        -1.0,
        1.0
    );
    glm::vec4 lRayEnd_NDC(
        ((float)mouse_x / (float)Settings::Instance()->SDL_Window_Width  - 0.5f) * 2.0f,
        ((float)mouse_y / (float)Settings::Instance()->SDL_Window_Height - 0.5f) * 2.0f,
        0.0,
        1.0
    );

    glm::mat4 InverseProjectionMatrix = glm::inverse(this->managerObjects->matrixProjection);
    glm::mat4 InverseViewMatrix = glm::inverse(this->managerObjects->camera->matrixCamera);

    glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;
    lRayStart_camera /= lRayStart_camera.w;

    glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera;
    lRayStart_world /= lRayStart_world .w;

    glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;
    lRayEnd_camera /= lRayEnd_camera.w;

    glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;
    lRayEnd_world /= lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = glm::normalize(lRayDir_world);

    glm::vec3 out_origin = glm::vec3(lRayStart_world);
    glm::vec3 out_direction = glm::normalize(lRayDir_world);
    glm::vec3 out_end = out_origin + out_direction * 1000.0f;

    this->sceneSelectedModelObject = -1;
    this->selectedMaterialID = "";
    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 tp1 = mmf->oFace.vectors_vertices[j];
                glm::vec3 tp2 = mmf->oFace.vectors_vertices[j - 1];
                glm::vec3 tp3 = mmf->oFace.vectors_vertices[j - 2];

                glm::vec3 intersectionPoint;
                if (glm::intersectLineTriangle(out_origin, out_direction, tp1, tp2, tp3, intersectionPoint)) {
                    this->sceneSelectedModelObject = i;
                    this->selectedMaterialID = mmf->oFace.materialID;
                    this->doLog(Settings::Instance()->string_format("!!!! HIT !!!! %s @ %f, %f, %f\n", mmf->oFace.ModelTitle.c_str(), intersectionPoint.x, intersectionPoint.y, intersectionPoint.z));
                }
            }
        }
    }
}

void RayPicking::pick4() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    // picking #2
    glm::vec2 normalizedCoordinates = this->getNormalizeDeviceCordinates(mouse_x, mouse_y);
    glm::vec4 clipCoordinates = glm::vec4(normalizedCoordinates, -1.0f, 1.0f);
    glm::vec4 eyeCoordinates = this->getEyeCoordinates(clipCoordinates);
    glm::vec3 worldRay = this->getWorldCoordinates(eyeCoordinates);

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glm::vec3 win_near = glm::vec3(mouse_x, Settings::Instance()->SDL_Window_Height - mouse_y, 0.0);
    glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);

    glm::vec3 vFrom = worldRay;
    glm::vec3 vTo = glm::vec3(0);

    RayLine *rl = new RayLine();
    rl->init();
    rl->initShaderProgram();
    rl->initBuffers(vFrom, vTo);
    this->rayLines.push_back(rl);

    this->sceneSelectedModelObject = -1;
    this->selectedMaterialID = "";
    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 tp1 = mmf->oFace.vectors_vertices[j];
                glm::vec3 tp2 = mmf->oFace.vectors_vertices[j - 1];
                glm::vec3 tp3 = mmf->oFace.vectors_vertices[j - 2];

                glm::vec3 intersectionPoint;
                if (this->isInTriangle(nearPoint, worldRay, tp1, tp2, tp3)) {
                    this->sceneSelectedModelObject = i;
                    this->selectedMaterialID = mmf->oFace.materialID;
                    this->doLog(Settings::Instance()->string_format("!!!! HIT !!!! %s @ %f, %f, %f\n", mmf->oFace.ModelTitle.c_str(), intersectionPoint.x, intersectionPoint.y, intersectionPoint.z));
                }
            }
        }
    }
}

void RayPicking::pick3() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    // picking #1
    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);

    glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);
    glm::vec3 farPoint = glm::unProject(win_far, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);

    glm::vec3 win_near = glm::vec3(mouse_x, Settings::Instance()->SDL_Window_Height - mouse_y, 0.0);
    glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);

    glm::vec3 rayDirection = glm::normalize(farPoint - nearPoint);

    glm::vec3 vFrom = glm::vec3(0.0f);
    glm::vec3 vTo = glm::vec3(0.0f);

    vFrom = nearPoint;
    vFrom = glm::vec3(this->managerObjects->cameraModel->positionX->point, this->managerObjects->cameraModel->positionY->point, this->managerObjects->cameraModel->positionZ->point);
    vFrom = glm::vec3(this->managerObjects->camera->positionX->point, this->managerObjects->camera->positionY->point, this->managerObjects->camera->positionZ->point);
    vFrom = glm::vec3(this->managerObjects->camera->matrixCamera[3].x, this->managerObjects->camera->matrixCamera[3].y, - this->managerObjects->camera->matrixCamera[3].z);
    vTo = farPoint;

    RayLine *rl = new RayLine();
    rl->init();
    rl->initShaderProgram();
    rl->initBuffers(vFrom, vTo);
    this->rayLines.push_back(rl);
    this->doLog(Settings::Instance()->string_format("%f, %f, %f <--------> %f, %f, %f", vFrom.x, vFrom.y, vFrom.z, vTo.x, vTo.y, vTo.z));

    this->sceneSelectedModelObject = -1;
    this->selectedMaterialID = "";
    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 triangle_p1 = mmf->oFace.vectors_vertices[j];
                glm::vec3 triangle_p2 = mmf->oFace.vectors_vertices[j - 1];
                glm::vec3 triangle_p3 = mmf->oFace.vectors_vertices[j - 2];

                std::string test = Settings::Instance()->string_format("triangle - (%.2f, %.2f, %.2f), ", triangle_p1.x, triangle_p1.y, triangle_p1.z);
                test += Settings::Instance()->string_format("(%.2f, %.2f, %.2f), ", triangle_p2.x, triangle_p2.y, triangle_p2.z);
                test += Settings::Instance()->string_format("(%.2f, %.2f, %.2f) ", triangle_p3.x, triangle_p3.y, triangle_p3.z);
                test += Settings::Instance()->string_format("with ray - ");
                test += Settings::Instance()->string_format("(%.2f, %.2f, %.2f), ", nearPoint.x, nearPoint.y, nearPoint.z);
                test += Settings::Instance()->string_format("(%.2f, %.2f, %.2f)", farPoint.x, farPoint.y, farPoint.z);
                glm::vec3 intersectionPoint;
                if (glm::intersectLineTriangle(nearPoint, rayDirection, triangle_p1, triangle_p2, triangle_p3, intersectionPoint)) {
                    this->sceneSelectedModelObject = i;
                    this->selectedMaterialID = mmf->oFace.materialID;
                    this->doLog(test);
                    this->doLog(Settings::Instance()->string_format("!!!! HIT !!!! %s @ %f, %f, %f", mmf->oFace.ModelTitle.c_str(), intersectionPoint.x, intersectionPoint.y, intersectionPoint.z));
                }
            }
        }
    }
}

bool RayPicking::isInTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 barycentricIntersect;
    bool doesIntersect = glm::intersectRayTriangle(origin, normalize(dir), p1, p2, p3, barycentricIntersect);
    this->doLog(Settings::Instance()->string_format("Bary: %f, %f, %f, %s", barycentricIntersect.x, barycentricIntersect.y, barycentricIntersect.z, std::to_string(doesIntersect).c_str()));
    return doesIntersect;
}

glm::vec2 RayPicking::getNormalizeDeviceCordinates(float X, float Y) {
    float x = (2 * X) / Settings::Instance()->SDL_Window_Width - 1;
    float y = (2 * Y) / Settings::Instance()->SDL_Window_Height - 1;
    return glm::vec2(x, -y);
}

glm::vec4 RayPicking::getEyeCoordinates(glm::vec4& coordinates) {
    glm::mat4 invertedProjectionMatrix = glm::inverse(this->managerObjects->matrixProjection);
    glm::vec4 eyeCoordinates = invertedProjectionMatrix * coordinates;
    return glm::vec4(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
}

glm::vec3 RayPicking::getWorldCoordinates(glm::vec4& coordinates) {
    glm::mat4 invertedViewMatrix = glm::inverse(this->managerObjects->camera->matrixCamera);
    glm::vec4 rayWorld = invertedViewMatrix * coordinates;
    glm::vec3 mouseRay = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
    mouseRay = glm::normalize(mouseRay);
    return mouseRay;
}

void RayPicking::pick2() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    // Color picking
    PixelDataPoint p = this->managerObjects->camera->getClickData(mouse_x, mouse_y, Settings::Instance()->SDL_Window_Height);
    this->doLog(Settings::Instance()->string_format("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u", mouse_x, mouse_y, p.color[0], p.color[1], p.color[2], p.color[3], p.depth, p.index));

    // ----- picking
    glm::vec3 rayDirection = this->managerObjects->camera->createRay(
                                 this->managerControls->mousePosition.x,
                                 this->managerControls->mousePosition.y,
                                 this->managerObjects->Setting_FOV,
                                 this->managerObjects->Setting_RatioWidth / this->managerObjects->Setting_RatioHeight,
                                 this->managerObjects->Setting_PlaneClose,
                                 this->managerObjects->Setting_PlaneFar
    );

    // Values you might be interested:
    float someDistance = 1.0;
    glm::vec3 rayEndPosition = this->managerObjects->camera->cameraPosition + rayDirection * someDistance;
    this->doLog(Settings::Instance()->string_format("[Ray Direction] @ %f - %f - %f", rayDirection.x, rayDirection.y, rayDirection.z));
    this->doLog(Settings::Instance()->string_format("PICKED @ %f - %f - %f", rayEndPosition.x, rayEndPosition.y, rayEndPosition.z));

    // ----- picking
    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glm::vec3 win_near = glm::vec3(mouse_x, mouse_y, 0.0);
    glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);

    glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 farPoint = glm::unProject(win_far, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 direction = glm::normalize(farPoint - nearPoint);

    // http://schabby.de/picking-opengl-ray-tracing/
    // http://stackoverflow.com/questions/27891036/dragging-3-dimensional-objects-with-c-and-opengl
    float sceneClosestObject = -1;

    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        std::vector<glm::vec3> vertices;

//                std::vector<float> objVertices = mmf->oFace.vertices;
//                for (size_t j=0; j<objVertices.size(); j++) {
//                    if ((j + 1) % 3 == 0)
//                        vertices.push_back(glm::vec3(objVertices[j], objVertices[j - 1], objVertices[j - 2]));
//                }

        glm::mat4 m = mmf->matrixProjection * mmf->matrixCamera * mmf->matrixModel;
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            float x = mmf->oFace.vectors_vertices[j].x;
            float y = mmf->oFace.vectors_vertices[j].y;
            float z = mmf->oFace.vectors_vertices[j].z;
            glm::vec4 v = m * glm::vec4(x, y, z, 1.0);
            vertices.push_back(glm::vec3(v.x, v.y, v.z));
        }

        for (size_t j=0; j<vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 face_normal = glm::normalize(glm::cross(vertices[j - 1] - vertices[j - 2], vertices[j] - vertices[j - 2]));

                float nDotL = glm::dot(direction, face_normal);
                if (nDotL <= 0.0f) {
                    float distance = glm::dot(face_normal, (vertices[j - 2] - nearPoint)) / nDotL;

                    glm::vec3 p = nearPoint + distance * direction;
                    glm::vec3 n1 = glm::cross(vertices[j - 1] - vertices[j - 2], p - vertices[j - 2]);
                    glm::vec3 n2 = glm::cross(vertices[j] - vertices[j - 1], p - vertices[j - 1]);
                    glm::vec3 n3 = glm::cross(vertices[j - 2] - vertices[j], p - vertices[j]);
                    if (glm::dot(face_normal, n1) >= 0.0f && glm::dot(face_normal, n2) >= 0.0f && glm::dot(face_normal, n3) >= 0.0f) {
                        if (p.z > sceneClosestObject) {
                            this->sceneSelectedModelObject = i;
                            this->selectedMaterialID = mmf->oFace.materialID;
                            this->doLog("RayCast @ [" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + "] = [" + std::to_string(this->sceneSelectedModelObject) + "] - " + this->selectedMaterialID);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void RayPicking::pick1() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    // Color picking
    PixelDataPoint p = this->managerObjects->camera->getClickData(mouse_x, mouse_y, Settings::Instance()->SDL_Window_Height);
    this->doLog(Settings::Instance()->string_format("Clicked on pixel %d, %d, color %02hhx%02hhx%02hhx%02hhx, depth %f, stencil index %u", mouse_x, mouse_y, p.color[0], p.color[1], p.color[2], p.color[3], p.depth, p.index));

    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glm::vec3 win_near = glm::vec3(mouse_x, mouse_y, 0.0);
    glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);

    glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 farPoint = glm::unProject(win_far, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 direction = glm::normalize(farPoint - nearPoint);

    // http://schabby.de/picking-opengl-ray-tracing/
    // http://stackoverflow.com/questions/27891036/dragging-3-dimensional-objects-with-c-and-opengl
    float sceneClosestObject = -1;

    for (int i=0; i<(int)meshModelFaces.size(); i++) {
        ModelFace *mmf = meshModelFaces[i];
        std::vector<glm::vec3> vertices;

//                std::vector<float> objVertices = mmf->oFace.vertices;
//                for (size_t j=0; j<objVertices.size(); j++) {
//                    if ((j + 1) % 3 == 0)
//                        vertices.push_back(glm::vec3(objVertices[j], objVertices[j - 1], objVertices[j - 2]));
//                }

        glm::mat4 m = mmf->matrixProjection * mmf->matrixCamera * mmf->matrixModel;
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            float x = mmf->oFace.vectors_vertices[j].x;
            float y = mmf->oFace.vectors_vertices[j].y;
            float z = mmf->oFace.vectors_vertices[j].z;
            glm::vec4 v = m * glm::vec4(x, y, z, 1.0);
            vertices.push_back(glm::vec3(v.x, v.y, v.z));
        }

        for (size_t j=0; j<vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 face_normal = glm::normalize(glm::cross(vertices[j - 1] - vertices[j - 2], vertices[j] - vertices[j - 2]));

                float nDotL = glm::dot(direction, face_normal);
                if (nDotL <= 0.0f) {
                    float distance = glm::dot(face_normal, (vertices[j - 2] - nearPoint)) / nDotL;

                    glm::vec3 p = nearPoint + distance * direction;
                    glm::vec3 n1 = glm::cross(vertices[j - 1] - vertices[j - 2], p - vertices[j - 2]);
                    glm::vec3 n2 = glm::cross(vertices[j] - vertices[j - 1], p - vertices[j - 1]);
                    glm::vec3 n3 = glm::cross(vertices[j - 2] - vertices[j], p - vertices[j]);
                    if (glm::dot(face_normal, n1) >= 0.0f && glm::dot(face_normal, n2) >= 0.0f && glm::dot(face_normal, n3) >= 0.0f) {
                        if (p.z > sceneClosestObject) {
                            this->sceneSelectedModelObject = i;
                            this->selectedMaterialID = mmf->oFace.materialID;
                            this->doLog("RayCast @ [" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + "] = [" + std::to_string(this->sceneSelectedModelObject) + "] - " + this->selectedMaterialID);
                            break;
                        }
                    }
                }
            }
        }
    }
}

RayPicking::~RayPicking() {
    this->destroy();
}

void RayPicking::destroy() {
}

void RayPicking::init(ObjectsManager *managerObjects, Controls *managerControls, std::function<void(std::string)> doLog) {
    this->managerObjects = managerObjects;
    this->managerControls = managerControls;
    this->doLog = doLog;
}
