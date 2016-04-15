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
    void selectModel(std::vector<ModelFace*> meshModelFaces, std::vector<RayLine*> * rayLines, int *sceneSelectedModelObject, std::string *selectedMaterialID);

    void pick1();
    void pick2();
    void pick3();
    void pick4();
    void pick5();

private:
    std::function<void(std::string)> doLog;
    ObjectsManager *managerObjects;
    Controls *managerControls;

    std::vector<ModelFace*> meshModelFaces;
    std::vector<RayLine*> rayLines;
    int sceneSelectedModelObject;
    std::string selectedMaterialID;

    void destroy();

    glm::vec2 getNormalizeDeviceCordinates(float X, float Y);
    glm::vec4 getEyeCoordinates(glm::vec4& coordinates);
    glm::vec3 getWorldCoordinates(glm::vec4& coordinates);
    bool isInTriangle(glm::vec3 origin, glm::vec3 dir, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};

#endif /* RayPicking_hpp */
