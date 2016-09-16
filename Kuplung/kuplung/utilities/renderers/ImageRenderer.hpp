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
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

typedef enum ImageRendererType {
    ImageRendererType_Scene,
    ImageRendererType_RayTrace
} ImageRendererType;

class ImageRenderer {
public:
    ~ImageRenderer();
    ImageRenderer();
    void init();
    void destroy();
    void renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects);

private:
    SceneRenderer *rendererScene;
};

#endif /* ImageRenderer_hpp */
