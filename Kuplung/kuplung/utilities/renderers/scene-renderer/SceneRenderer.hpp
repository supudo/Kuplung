//
//  SceneRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef SceneRenderer_hpp
#define SceneRenderer_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/gl/GLUtils.hpp"

class SceneRenderer {
public:
    ~SceneRenderer();
    void init();
    void renderImage(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects);
    void renderImage2(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects);
};

#endif /* SceneRenderer_hpp */
