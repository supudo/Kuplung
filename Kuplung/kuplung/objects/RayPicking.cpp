//
//  RayPicking.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RayPicking.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/intersect.hpp>

RayPicking::~RayPicking() {
    this->destroy();
}

void RayPicking::destroy() {
}

void RayPicking::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
}

void RayPicking::setMatrices(glm::mat4 matrixProjection, glm::mat4 matrixCamera) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
}

void RayPicking::selectModel(std::vector<ModelFaceBase*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject,
                             std::unique_ptr<ObjectsManager> &managerObjects, std::unique_ptr<Controls> &managerControls) {
    this->meshModelFaces = meshModelFaces;
    this->sceneSelectedModelObject = *sceneSelectedModelObject;
    this->pick(managerObjects, managerControls);
    *sceneSelectedModelObject = this->sceneSelectedModelObject;
    *rayLines = this->rayLines;
}

void RayPicking::pick(std::unique_ptr<ObjectsManager> &managerObjects, std::unique_ptr<Controls> &managerControls) {
    int mouse_x = managerControls->mousePosition.x;
    int mouse_y = managerControls->mousePosition.y;

    glm::vec3 vFrom;
    glm::vec3 ray_direction;
    this->getRay(
        Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2,
        Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
        this->matrixCamera,
        this->matrixProjection,
        vFrom,
        ray_direction
    );

    glm::vec2 normalizedCoordinates = this->getNormalizeDeviceCordinates(mouse_x, mouse_y);
    glm::vec4 clipCoordinates = glm::vec4(normalizedCoordinates, -1.0f, 1.0f);
    glm::vec4 eyeCoordinates = this->getEyeCoordinates(clipCoordinates, managerObjects);

    glm::mat4 invertedViewMatrix = glm::inverse(managerObjects->camera->matrixCamera);
    glm::vec4 rayWorld = invertedViewMatrix * eyeCoordinates;
    glm::vec3 vTo = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);

    if (Settings::Instance()->showPickRays) {
        RayLine *rl = new RayLine();
        rl->init();
        rl->initShaderProgram();
        rl->initBuffers(vFrom, vTo * managerObjects->Setting_PlaneFar);
        this->rayLines.push_back(rl);
    }

    this->sceneSelectedModelObject = -1;
    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFaceBase *mmf = this->meshModelFaces[i];
        for (size_t j=0; j<mmf->meshModel.vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
//                glm::vec3 v1 = this->fixSignVector(mmf->oFace.vectors_vertices[j]);
//                glm::vec3 v2 = this->fixSignVector(mmf->oFace.vectors_vertices[j - 1]);
//                glm::vec3 v3 = this->fixSignVector(mmf->oFace.vectors_vertices[j - 2]);

//                glm::vec4 tp01 = mmf->matrixModel * glm::vec4(v1, 1.0);
//                glm::vec4 tp02 = mmf->matrixModel * glm::vec4(v2, 1.0);
//                glm::vec4 tp03 = mmf->matrixModel * glm::vec4(v3, 1.0);

//                glm::vec3 tp1 = this->fixSignVector(glm::vec3(tp01.x, tp01.y, tp01.z));
//                glm::vec3 tp2 = this->fixSignVector(glm::vec3(tp02.x, tp02.y, tp02.z));
//                glm::vec3 tp3 = this->fixSignVector(glm::vec3(tp03.x, tp03.y, tp03.z));

//                glm::vec3 intersectionPoint;
//                if (glm::intersectLineTriangle(vFrom, vTo * this->managerObjects->Setting_PlaneFar, tp1, tp2, tp3, intersectionPoint)) {
//                    this->sceneSelectedModelObject = i;
//                    this->doLog(Settings::Instance()->string_format("[Pick] %s", mmf->oFace.ModelTitle.c_str()));
//                }

                float id = -1;
                glm::vec3 aabb_min = glm::vec3(mmf->boundingBox->min_x, mmf->boundingBox->min_y, mmf->boundingBox->min_z);
                glm::vec3 aabb_max = glm::vec3(mmf->boundingBox->max_x, mmf->boundingBox->max_y, mmf->boundingBox->max_z);
                if (this->testRayOBBIntersection(vFrom, vTo, aabb_min, aabb_max, mmf->matrixModel, id)) {
                    this->sceneSelectedModelObject = i;
                    //this->doLog(Settings::Instance()->string_format("[Pick] %s", mmf->oFace.ModelTitle.c_str()));
                }
            }
        }
    }
}

glm::vec2 RayPicking::getNormalizeDeviceCordinates(float X, float Y) {
    float x = (2 * X) / Settings::Instance()->SDL_Window_Width - 1;
    float y = (2 * Y) / Settings::Instance()->SDL_Window_Height - 1;
    return glm::vec2(x, -y);
}

glm::vec4 RayPicking::getEyeCoordinates(glm::vec4& coordinates, std::unique_ptr<ObjectsManager> &managerObjects) {
    glm::mat4 invertedProjectionMatrix = glm::inverse(managerObjects->matrixProjection);
    glm::vec4 eyeCoordinates = invertedProjectionMatrix * coordinates;
    return glm::vec4(eyeCoordinates.x, eyeCoordinates.y, -1.0f, 0.0f);
}

bool RayPicking::testRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance) {
    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if (fabs(f) > 0.001f) { // Standard case
            float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
            float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if (t2 < tMax)
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if (t1 > tMin)
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin)
                return false;

        }
        else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + aabb_min.y) / f;
            float t2 = (e + aabb_max.y) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;
        }
        else {
            if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
                return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if (fabs(f) > 0.001f) {

            float t1 = (e + aabb_min.z) / f;
            float t2 = (e + aabb_max.z) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax)
                tMax = t2;
            if (t1 > tMin)
                tMin = t1;
            if (tMin > tMax)
                return false;
        }
        else {
            if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
                return false;
        }
    }
    intersection_distance = tMin;
    return true;
}

void RayPicking::getRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3& out_origin, glm::vec3& out_direction){
    glm::vec4 lRayStart_NDC(
        ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
        ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
        -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
        1.0f
    );
    glm::vec4 lRayEnd_NDC(
        ((float)mouseX/(float)screenWidth  - 0.5f) * 2.0f,
        ((float)mouseY/(float)screenHeight - 0.5f) * 2.0f,
        0.0,
        1.0f
    );

    // The Projection matrix goes from Camera Space to NDC.
    // So inverse(ProjectionMatrix) goes from NDC to Camera Space.
//    glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

    // The View Matrix goes from World Space to Camera Space.
    // So inverse(ViewMatrix) goes from Camera Space to World Space.
//    glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

//    glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
//    glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
//    glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
//    glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;

    // Faster way (just one inverse)
    glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
    glm::vec4 lRayStart_world = M * lRayStart_NDC;
    lRayStart_world /= lRayStart_world.w;
    glm::vec4 lRayEnd_world = M * lRayEnd_NDC;
    lRayEnd_world /=lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = glm::normalize(lRayDir_world);

    out_origin = glm::vec3(lRayStart_world);
    out_direction = glm::normalize(lRayDir_world);
}

float RayPicking::fixSign(float num) {
    if ((num < 0.0f) && (-log10(std::abs(num)) > FLT_EPSILON))
        return -num;
    else
        return num;
}

glm::vec3 RayPicking::fixSignVector(glm::vec3 v) {
    return glm::vec3(this->fixSign(v.x), this->fixSign(v.y), this->fixSign(v.z));
}
