//
//  ImageRenderer.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "ImageRenderer.hpp"

ImageRenderer::~ImageRenderer() {
    this->destroy();
}

ImageRenderer::ImageRenderer() {
    this->rendererScene = std::make_unique<SceneRenderer>();
}

void ImageRenderer::destroy() {
}

void ImageRenderer::init() {
    this->rendererScene->init();
}

void ImageRenderer::renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces, std::unique_ptr<ObjectsManager> &managerObjects) {
    if (type == ImageRendererType_Scene)
        this->rendererScene->renderImage(file, meshModelFaces, managerObjects);
}
