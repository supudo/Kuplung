//
//  RayPicking.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/3/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RayPicking_hpp
#define RayPicking_hpp

#include "kuplung/utilities/gl/GLIncludes.h"
#include "kuplung/settings/Settings.h"
#include "kuplung/ui/UI.hpp"
#include "kuplung/meshes/ModelFace.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/input/Controls.hpp"
#include "kuplung/meshes/RayLine.hpp"

class RayPicking {
public:
    ~RayPicking();
    void init(ObjectsManager *managerObjects, Controls *managerControls, std::function<void(std::string)> doLog);
    void setMatrices(glm::mat4 matrixProjection, glm::mat4 matrixCamera);
    void selectModel(std::vector<ModelFace*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject);

    std::vector<RayLine*> rayLines;

    void pick1();
    void pick2();
    void pick3();
    void pick4();
    void pick5();
    void pick6();
    void pick7();

private:
    std::function<void(std::string)> doLog;
    ObjectsManager *managerObjects;
    Controls *managerControls;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;

    std::vector<ModelFace*> meshModelFaces;
    int sceneSelectedModelObject;

    void destroy();

    glm::vec2 getNormalizeDeviceCordinates(float X, float Y);
    glm::vec4 getEyeCoordinates(glm::vec4& coordinates);
    glm::vec3 getWorldCoordinates(glm::vec4& coordinates);
    bool isInTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    void ScreenPosToWorldRay(
        int mouseX, int mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
        int screenWidth, int screenHeight,  // Window size, in pixels
        glm::mat4 ViewMatrix,               // Camera position and orientation
        glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
        glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
        glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
    );
    bool TestRayOBBIntersection(
        glm::vec3 ray_origin,        // Ray origin, in world space
        glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
        glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
        glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
        glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
        float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
    );
    float fixSign(float num);
    glm::vec3 fixSignVector(glm::vec3 v);
    int rayIntersectsTriangle(float *p, float *d, float *v0, float *v1, float *v2);
};

#endif /* RayPicking_hpp */
