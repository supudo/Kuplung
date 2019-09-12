//
//  RayTracerRenderer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef RayTracerRenderer_hpp
#define RayTracerRenderer_hpp

#include "kuplung/settings/Settings.h"
#include "kuplung/meshes/scene/ModelFaceBase.hpp"
#include "kuplung/meshes/scene/ModelFaceData.hpp"
#include "kuplung/objects/ObjectsManager.hpp"
#include "kuplung/utilities/renderers/KuplungRendererBase.hpp"

class RayTracerRenderer: public KuplungRendererBase {
public:
  explicit RayTracerRenderer(ObjectsManager &managerObjects);
  ~RayTracerRenderer();
  void init();
  std::string renderImage(FBEntity const& file, std::vector<ModelFaceBase*> *meshModelFaces);

private:
  FBEntity fileOutputImage;

  ObjectsManager &managerObjects;

  GLuint renderFBO, renderRBO, renderTextureColorBuffer;
  void createFBO();
  void generateAttachmentTexture(GLboolean depth, GLboolean stencil);
  void renderSceneToFBO(std::vector<ModelFaceBase*> *meshModelFaces) const;
};

#endif /* RayTracerRenderer_hpp */
