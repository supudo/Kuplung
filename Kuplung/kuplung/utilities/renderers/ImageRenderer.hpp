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
#include "kuplung/utilities/renderers/default-forward/DefaultForwardRenderer.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

typedef enum ImageRendererType {
    ImageRendererType_Scene,
    ImageRendererType_DefaultForward
} ImageRendererType;

class ImageRenderer {
public:
    explicit ImageRenderer(ObjectsManager &managerObjects);
    ~ImageRenderer();
    void init();
    std::string renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces);
    void showSpecificSettings(ImageRendererType type);

private:
    ObjectsManager &managerObjects;
    std::unique_ptr<SceneRenderer> rendererScene;
    std::unique_ptr<DefaultForwardRenderer> rendererDefaultForward;
};

#endif /* ImageRenderer_hpp */
