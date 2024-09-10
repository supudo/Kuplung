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
#include "kuplung/utilities/renderers/ray-tracer/RayTracerRenderer.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

typedef enum ImageRendererType {
  ImageRendererType_Scene,
  ImageRendererType_DefaultForward,
  ImageRendererType_DefaultForwardShadowMapping,
  ImageRendererType_RayTracer
} ImageRendererType;

class ImageRenderer {
public:
  explicit ImageRenderer(ObjectsManager &managerObjects);
  void init();
  const std::string renderImage(ImageRendererType type, const FBEntity& file, std::vector<ModelFaceBase*> *meshModelFaces) const;
  void showSpecificSettings(ImageRendererType type) const;

private:
  ObjectsManager &managerObjects;
  std::unique_ptr<SceneRenderer> rendererScene;
  std::unique_ptr<DefaultForwardRenderer> rendererDefaultForward;
  std::unique_ptr<RayTracerRenderer> rendererRayTracer;
};

#endif /* ImageRenderer_hpp */
