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
#include "kuplung/meshes/scene/Model.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/input/Controls.hpp"
#include "kuplung/meshes/helpers/RayLine.hpp"

class RayPicking {
public:
    ~RayPicking();
    void init(ObjectsManager *managerObjects, Controls *managerControls, std::function<void(std::string)> doLog);
    void setMatrices(glm::mat4 matrixProjection, glm::mat4 matrixCamera);
    void selectModel(std::vector<Model*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject);

    std::vector<RayLine*> rayLines;

private:
    std::function<void(std::string)> doLog;
    ObjectsManager *managerObjects;
    Controls *managerControls;

    glm::mat4 matrixProjection;
    glm::mat4 matrixCamera;

    std::vector<Model*> meshModelFaces;
    int sceneSelectedModelObject;

    void destroy();
    void pick();

    glm::vec2 getNormalizeDeviceCordinates(float X, float Y);
    glm::vec4 getEyeCoordinates(glm::vec4& coordinates);
    void getRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3& out_origin, glm::vec3& out_direction);
    bool testRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance);
    float fixSign(float num);
    glm::vec3 fixSignVector(glm::vec3 v);
};

#endif /* RayPicking_hpp */
