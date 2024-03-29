//
//  KuplungRendererBase.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungRendererBase_hpp
#define KuplungRendererBase_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"

class KuplungRendererBase {
public:
  virtual ~KuplungRendererBase() = default;
  virtual void init();
  virtual std::string renderImage(const FBEntity& file, std::vector<ModelFaceBase*> *meshModelFaces);
  virtual void showSpecificSettings();

  bool Setting_RenderSkybox;
};

#endif /* KuplungRendererBase_hpp */
