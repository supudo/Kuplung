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
#include "kuplung/utilities/renderers/KuplungRendererBase.hpp"

class SceneRenderer: public KuplungRendererBase {
public:
  explicit SceneRenderer(ObjectsManager &managerObjects);
  void init();
  const std::string renderImage(const FBEntity& file, std::vector<ModelFaceBase*> *meshModelFaces) const;
  const std::string renderImage2(const FBEntity& file, std::vector<ModelFaceBase*> *meshModelFaces) const;

private:
  ObjectsManager &managerObjects;
};

#endif /* SceneRenderer_hpp */
