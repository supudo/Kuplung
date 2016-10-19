//
//  RenderingManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/2/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "RenderingManager.hpp"

RenderingManager::RenderingManager(ObjectsManager &managerObjects) : managerObjects(managerObjects) {
    this->managerObjects = managerObjects;
}

RenderingManager::~RenderingManager() {
}

void RenderingManager::init() {
    this->rendererSimple = std::make_unique<RenderingSimple>(this->managerObjects);
    this->rendererSimple->init();

    this->rendererForward = std::make_unique<RenderingForward>(this->managerObjects);
    this->rendererForward->init();

    this->rendererDeferred = std::make_unique<RenderingDeferred>(this->managerObjects);
    this->rendererDeferred->init();

    this->RenderingTotalVertices = 0;
    this->RenderingTotalIndices = 0;
    this->RenderingTotalTriangles = 0;
    this->RenderingTotalFaces = 0;
}

void RenderingManager::render(int selectedModel) {
    switch (Settings::Instance()->RendererType) {
        case 0:
            this->rendererSimple->render(this->meshModelFaces, selectedModel);
            break;
        case 1:
            this->rendererForward->render(this->meshModelFaces, selectedModel);
            break;
        case 2:
            this->rendererDeferred->render(this->meshModelFaces, selectedModel);
            break;
        default:
            break;
    }
    this->RenderingTotalVertices = 0;
    this->RenderingTotalIndices = 0;
    this->RenderingTotalTriangles = 0;
    this->RenderingTotalFaces = 0;
    for (size_t i=0; i<this->meshModelFaces.size(); i++) {
        ModelFaceBase* mmf = this->meshModelFaces[i];
        this->RenderingTotalVertices += mmf->meshModel.countVertices;
        if (mmf->meshModel.indices.size() > 0)
            this->RenderingTotalIndices += ((*std::max_element(mmf->meshModel.indices.begin(), mmf->meshModel.indices.end())) + 1);
        this->RenderingTotalTriangles += this->RenderingTotalVertices / 3;
        this->RenderingTotalFaces += this->RenderingTotalTriangles / 2;
    }

    Settings::Instance()->sceneCountObjects = int(this->meshModelFaces.size());
    Settings::Instance()->sceneCountVertices = this->RenderingTotalVertices;
    Settings::Instance()->sceneCountIndices = this->RenderingTotalIndices;
    Settings::Instance()->sceneCountTriangles = this->RenderingTotalTriangles;
    Settings::Instance()->sceneCountFaces = this->RenderingTotalFaces;
}

