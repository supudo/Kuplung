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

void RayPicking::selectModel(std::vector<ModelFace*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject) {
    this->meshModelFaces = meshModelFaces;
    this->sceneSelectedModelObject = *sceneSelectedModelObject;
    this->pick7();
    *sceneSelectedModelObject = this->sceneSelectedModelObject;
    *rayLines = this->rayLines;
}

void RayPicking::pick7() {
    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    glm::vec3 vFrom;
    glm::vec3 ray_direction;
    this->ScreenPosToWorldRay(
        Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2,
        Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
        this->matrixCamera,
        this->matrixProjection,
        vFrom,
        ray_direction
    );

    glm::vec2 normalizedCoordinates = this->getNormalizeDeviceCordinates(mouse_x, mouse_y);
    glm::vec4 clipCoordinates = glm::vec4(normalizedCoordinates, -1.0f, 1.0f);
    glm::vec4 eyeCoordinates = this->getEyeCoordinates(clipCoordinates);

    glm::mat4 invertedViewMatrix = glm::inverse(this->managerObjects->camera->matrixCamera);
    glm::vec4 rayWorld = invertedViewMatrix * eyeCoordinates;
    glm::vec3 vTo = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);

    RayLine *rl = new RayLine();
    rl->init();
    rl->initShaderProgram();
    rl->initBuffers(vFrom, vTo * this->managerObjects->Setting_PlaneFar);
    this->rayLines.push_back(rl);
    this->doLog(Settings::Instance()->string_format("[RAY] %f, %f, %f <--------> %f, %f, %f", vFrom.x, vFrom.y, vFrom.z, vTo.x, vTo.y, vTo.z));

    for (int i=0; i<(int)this->meshModelFaces.size(); i++) {
        ModelFace *mmf = this->meshModelFaces[i];
        for (size_t j=0; j<mmf->oFace.vectors_vertices.size(); j++) {
            if ((j + 1) % 3 == 0) {
                glm::vec3 v1 = this->fixSignVector(mmf->oFace.vectors_vertices[j]);
                glm::vec3 v2 = this->fixSignVector(mmf->oFace.vectors_vertices[j - 1]);
                glm::vec3 v3 = this->fixSignVector(mmf->oFace.vectors_vertices[j - 2]);

                glm::vec4 tp01 = mmf->matrixModel * glm::vec4(v1, 1.0);
                glm::vec4 tp02 = mmf->matrixModel * glm::vec4(v2, 1.0);
                glm::vec4 tp03 = mmf->matrixModel * glm::vec4(v3, 1.0);

                glm::vec3 tp1 = this->fixSignVector(glm::vec3(tp01.x, tp01.y, -tp01.z));
                glm::vec3 tp2 = this->fixSignVector(glm::vec3(tp02.x, tp02.y, -tp02.z));
                glm::vec3 tp3 = this->fixSignVector(glm::vec3(tp03.x, tp03.y, -tp03.z));

                glm::vec3 intersectionPoint;
                std::string intersectionStr = "";
                if (glm::intersectLineTriangle(vFrom, vTo * this->managerObjects->Setting_PlaneFar, tp1, tp2, tp3, intersectionPoint)) {
                    this->sceneSelectedModelObject = i;
                    intersectionStr += Settings::Instance()->string_format("!!!! HIT !!!! [%i] %s", this->sceneSelectedModelObject, mmf->oFace.ModelTitle.c_str());
                }
                intersectionStr += Settings::Instance()->string_format(" -- %f, %f, %f", intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
                this->doLog(intersectionStr);
            }
        }
    }
}

#define vector(a,b,c) \
    (a)[0] = (b)[0] - (c)[0];	\
    (a)[1] = (b)[1] - (c)[1];	\
    (a)[2] = (b)[2] - (c)[2];

#define crossProduct(a,b,c) \
    (a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
    (a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
    (a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];

#define innerProduct(v,q) \
        ((v)[0] * (q)[0] + \
        (v)[1] * (q)[1] + \
        (v)[2] * (q)[2])

int RayPicking::rayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2) {
    float e1[3], e2[3], h[3], s[3], q[3];
    float a, f, u, v;

    vector(e1, v1, v0);
    vector(e2, v2, v0);

    crossProduct(h, d, e2);
    a = innerProduct(e1, h);

    this->doLog(Settings::Instance()->string_format("XXXX = %f\n", a));

   if (a > -0.00001 && a < 0.00001)
        return(false);

    f = 1/a;
    vector(s, p, v0);
    u = f * (innerProduct(s, h));

    if (u < 0.0 || u > 1.0)
        return(false);

    crossProduct(q, s, e1);
    v = f * innerProduct(d, q);

    if (v < 0.0 || u + v > 1.0)
        return(false);

    // at this stage we can compute t to find out where
    // the intersection point is on the line
    float t = f * innerProduct(e2, q);

    if (t > 0.00001) // ray intersection
        return(true);

    else // this means that there is a line intersection
         // but not a ray intersection
         return (false);
}

void RayPicking::pick6() {

    int mouse_x = this->managerControls->mousePosition.x;
    int mouse_y = this->managerControls->mousePosition.y;

    glm::vec3 ray_origin;
    glm::vec3 ray_direction;
    this->ScreenPosToWorldRay(
        Settings::Instance()->SDL_Window_Width / 2, Settings::Instance()->SDL_Window_Height / 2,
        Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height,
        this->matrixCamera,
        this->matrixProjection,
        ray_origin,
        ray_direction
    );
    ray_origin = this->fixSignVector(ray_origin);
    ray_direction = this->fixSignVector(ray_direction);

    glm::vec2 normalizedCoordinates = this->getNormalizeDeviceCordinates(mouse_x, mouse_y);
    glm::vec4 clipCoordinates = glm::vec4(normalizedCoordinates, -1.0f, 1.0f);
    glm::vec4 eyeCoordinates = this->getEyeCoordinates(clipCoordinates);

    glm::mat4 invertedViewMatrix = glm::inverse(this->managerObjects->camera->matrixCamera);
    glm::vec4 rayWorld = invertedViewMatrix * eyeCoordinates * this->managerObjects->Setting_PlaneFar;
    glm::vec3 vTo = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);

    RayLine *rl = new RayLine();
    rl->init();
    rl->initShaderProgram();
    rl->initBuffers(ray_origin, vTo * 1000.0f);
    this->rayLines.push_back(rl);

    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        ModelFace* mmf = this->meshModelFaces[i];

        float intersection_distance; // Output of TestRayOBBIntersection()
        glm::vec3 aabb_min(-1.0f, -1.0f, -1.0f);
        glm::vec3 aabb_max( 1.0f,  1.0f,  1.0f);

        if (this->TestRayOBBIntersection(
            ray_origin,
            ray_direction,
            aabb_min,
            aabb_max,
            mmf->matrixModel,
            intersection_distance)) {
            this->sceneSelectedModelObject = i;
            this->doLog(Settings::Instance()->string_format("INTERSECTION - %i\n", this->sceneSelectedModelObject));
            break;
        }
    }
}

bool RayPicking::TestRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance) {
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if ( fabs(f) > 0.001f ){ // Standard case

            float t1 = (e+aabb_min.x)/f; // Intersection with the "left" plane
            float t2 = (e+aabb_max.x)/f; // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1>t2){
                float w=t1;t1=t2;t2=w; // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
            if ( t2 < tMax )
                tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
            if ( t1 > tMin )
                tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin )
                return false;

        }else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
            if(-e+aabb_min.x > 0.0f || -e+aabb_max.x < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.y)/f;
            float t2 = (e+aabb_max.y)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.y > 0.0f || -e+aabb_max.y < 0.0f)
                return false;
        }
    }


    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if ( fabs(f) > 0.001f ){

            float t1 = (e+aabb_min.z)/f;
            float t2 = (e+aabb_max.z)/f;

            if (t1>t2){float w=t1;t1=t2;t2=w;}

            if ( t2 < tMax )
                tMax = t2;
            if ( t1 > tMin )
                tMin = t1;
            if (tMin > tMax)
                return false;

        }else{
            if(-e+aabb_min.z > 0.0f || -e+aabb_max.z < 0.0f)
                return false;
        }
    }

    intersection_distance = tMin;
    return true;

}

void RayPicking::ScreenPosToWorldRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3& out_origin, glm::vec3& out_direction){

    // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
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
    glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
    glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;

    glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
    lRayDir_world = glm::normalize(lRayDir_world);

    out_origin = glm::vec3(lRayStart_world);
    out_direction = glm::normalize(lRayDir_world);
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
//    /glm::vec3 out_end = out_origin + out_direction * 1000.0f;

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
                            this->doLog("RayCast @ [" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + "] = " + std::to_string(this->sceneSelectedModelObject));
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

    glm::vec2 normalizedCoordinates = this->getNormalizeDeviceCordinates(mouse_x, mouse_y);
    glm::vec4 clipCoordinates = glm::vec4(normalizedCoordinates, -1.0f, 1.0f);
    glm::vec4 eyeCoordinates = this->getEyeCoordinates(clipCoordinates);

    glm::mat4 invertedViewMatrix = glm::inverse(this->managerObjects->camera->matrixCamera);
    glm::vec4 rayWorld = invertedViewMatrix * eyeCoordinates;
    glm::vec3 vTo = glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);


    glm::vec4 viewport = glm::vec4(0.0f, 0.0f, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height);
    glm::vec3 win_near = glm::vec3(mouse_x, mouse_y, 0.0);
    glm::vec3 win_far = glm::vec3(mouse_x, mouse_y, 1.0);

    glm::vec3 nearPoint = glm::unProject(win_near, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 farPoint = glm::unProject(win_far, this->managerObjects->camera->matrixCamera, this->managerObjects->matrixProjection, viewport);
    glm::vec3 direction = glm::normalize(farPoint - nearPoint);

    // http://schabby.de/picking-opengl-ray-tracing/
    // http://stackoverflow.com/questions/27891036/dragging-3-dimensional-objects-with-c-and-opengl
    float sceneClosestObject = -1;

    RayLine *rl = new RayLine();
    rl->init();
    rl->initShaderProgram();
    rl->initBuffers(nearPoint, vTo * this->managerObjects->Setting_PlaneFar);
    this->rayLines.push_back(rl);
    farPoint = vTo * this->managerObjects->Setting_PlaneFar;

    for (int i=0; i<(int)meshModelFaces.size(); i++) {
        ModelFace *mmf = meshModelFaces[i];
        std::vector<glm::vec3> vertices = mmf->oFace.vectors_vertices;
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
                            this->doLog("RayCast @ [" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + "] = " + std::to_string(this->sceneSelectedModelObject));
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

void RayPicking::setMatrices(glm::mat4 matrixProjection, glm::mat4 matrixCamera) {
    this->matrixProjection = matrixProjection;
    this->matrixCamera = matrixCamera;
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
