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

void ImageRenderer::destroy() {
}

void ImageRenderer::init() {
    this->rendererScene = new SceneRenderer();
    this->rendererScene->init();
}

void ImageRenderer::renderImage(ImageRendererType type, FBEntity file) {
    if (type == ImageRendererType_Scene)
        this->rendererScene->renderImage(file);
}
