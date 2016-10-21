//
//  ImageRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef ImageRenderer_hpp
#define ImageRenderer_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/utilities/renderers/scene-renderer/SceneRenderer.hpp"
#include "kuplung/utilities/renderers/scenefull-renderer/SceneFullRenderer.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

typedef enum ImageRendererType {
    ImageRendererType_Scene,
    ImageRendererType_SceneFull,
    ImageRendererType_RayTrace
} ImageRendererType;

class ImageRenderer {
public:
    explicit ImageRenderer(ObjectsManager &managerObjects);
    ~ImageRenderer();
    void init();
    std::string renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);

private:
    ObjectsManager &managerObjects;
    std::unique_ptr<SceneRenderer> rendererScene;
    std::unique_ptr<SceneFullRenderer> rendererSceneFull;
};

#endif /* ImageRenderer_hpp */
