//
//  ImageRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ImageRenderer.hpp"

ImageRenderer::ImageRenderer(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
}

void ImageRenderer::init() {
  this->rendererScene = std::make_unique<SceneRenderer>(this->managerObjects);
  this->rendererScene->init();

  this->rendererDefaultForward = std::make_unique<DefaultForwardRenderer>(this->managerObjects);
  this->rendererDefaultForward->init();

  this->rendererRayTracer = std::make_unique<RayTracerRenderer>(this->managerObjects);
  this->rendererRayTracer->init();
}

const std::string ImageRenderer::renderImage(ImageRendererType type, const FBEntity& file, std::vector<ModelFaceBase*> *meshModelFaces) const {
  if (type == ImageRendererType_Scene)
    return this->rendererScene->renderImage(file, meshModelFaces);
  else if (type == ImageRendererType_DefaultForward)
    return this->rendererDefaultForward->renderImage(file, meshModelFaces);
  else if (type == ImageRendererType_RayTracer)
    return this->rendererRayTracer->renderImage(file, meshModelFaces);
  return "";
}

void ImageRenderer::showSpecificSettings(ImageRendererType type) const {
  if (type == ImageRendererType_Scene)
    return this->rendererScene->showSpecificSettings();
  else if (type == ImageRendererType_DefaultForward)
    return this->rendererDefaultForward->showSpecificSettings();
  else if (type == ImageRendererType_RayTracer)
    return this->rendererRayTracer->showSpecificSettings();
}
