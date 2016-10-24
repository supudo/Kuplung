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
#include "kuplung/utilities/renderers/KuplungRendererBase.hpp"

class SceneRenderer: public KuplungRendererBase {
public:
    SceneRenderer(ObjectsManager &managerObjects);
    ~SceneRenderer();
    void init();
    std::string renderImage(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);
    std::string renderImage2(FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);

private:
    ObjectsManager &managerObjects;
};

#endif /* SceneRenderer_hpp */
