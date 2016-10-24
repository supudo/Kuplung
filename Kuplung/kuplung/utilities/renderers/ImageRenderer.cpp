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
    this->rendererDefault = std::make_unique<DefaultRenderer>(this->managerObjects);
    this->rendererDefault->init();

    this->rendererScene = std::make_unique<SceneRenderer>(this->managerObjects);
    this->rendererScene->init();

    this->rendererSceneFull = std::make_unique<SceneFullRenderer>(this->managerObjects);
    this->rendererSceneFull->init();
}

std::string ImageRenderer::renderImage(ImageRendererType type, FBEntity file, std::vector<ModelFaceBase*> *meshModelFaces) {
    if (type == ImageRendererType_Default)
        return this->rendererDefault->renderImage(file, meshModelFaces);
    else if (type == ImageRendererType_Scene)
        return this->rendererScene->renderImage(file, meshModelFaces);
    else if (type == ImageRendererType_SceneFull)
        return this->rendererSceneFull->renderImage(file, meshModelFaces);
    return "";
}
