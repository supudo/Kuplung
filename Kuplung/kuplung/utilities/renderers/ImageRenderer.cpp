//
//  ImageRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ImageRenderer.hpp"

ImageRenderer::ImageRenderer(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
}

ImageRenderer::~ImageRenderer() {
}

void ImageRenderer::init() {
    this->rendererScene = std::make_unique<SceneRenderer>(this->managerObjects);
    this->rendererScene->init();

    this->rendererDefaultForward = std::make_unique<DefaultForwardRenderer>(this->managerObjects);
    this->rendererDefaultForward->init();

    this->rendererDefaultDeferred = std::make_unique<DefaultDeferredRenderer>(this->managerObjects);
    this->rendererDefaultDeferred->init();
}

std::string ImageRenderer::renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces) {
    if (type == ImageRendererType_Scene)
        return this->rendererScene->renderImage(file, meshModelFaces);
    else if (type == ImageRendererType_DefaultForward)
        return this->rendererDefaultForward->renderImage(file, meshModelFaces);
    else if (type == ImageRendererType_DefaultDeferred)
        return this->rendererDefaultDeferred->renderImage(file, meshModelFaces);
    return "";
}
