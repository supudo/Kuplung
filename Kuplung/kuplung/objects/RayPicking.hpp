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
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/input/Controls.hpp"
#include "kuplung/meshes/helpers/RayLine.hpp"

class RayPicking {
public:
    ~RayPicking();
    void selectModel(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera,
                     const std::vector<ModelFaceBase*>& meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject,
                     std::unique_ptr<ObjectsManager> &managerObjects, const std::unique_ptr<KuplungApp::Utilities::Input::Controls> &managerControls);
    void selectVertex(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera,
                      const std::vector<ModelFaceBase*>& meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject,
                      std::unique_ptr<ObjectsManager> &managerObjects, const std::unique_ptr<KuplungApp::Utilities::Input::Controls> &managerControls);

    std::vector<RayLine*> rayLines;

private:
    int sceneSelectedModelObject;

    void pickModel(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const std::vector<ModelFaceBase*>& meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects, const std::unique_ptr<KuplungApp::Utilities::Input::Controls> &managerControls);
    void pickVertex(const glm::mat4& matrixProjection, const glm::mat4& matrixCamera, const std::vector<ModelFaceBase*>& meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects, const std::unique_ptr<KuplungApp::Utilities::Input::Controls> &managerControls);

    glm::vec2 getNormalizeDeviceCordinates(float X, float Y);
    glm::vec4 getEyeCoordinates(glm::vec4& coordinates, std::unique_ptr<ObjectsManager> &managerObjects);
    void getRay(int mouseX, int mouseY, int screenWidth, int screenHeight, glm::mat4 ViewMatrix, glm::mat4 ProjectionMatrix, glm::vec3& out_origin, glm::vec3& out_direction);
    bool testRayOBBIntersection(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 aabb_min, glm::vec3 aabb_max, glm::mat4 ModelMatrix, float& intersection_distance);
    bool testRaySphereIntersection(int const vID, glm::vec3 const& ray_origin, glm::vec3 const& ray_direction, glm::vec3 const& vertex, glm::mat4 const& mtx, float const& radius);
};

#endif /* RayPicking_hpp */
