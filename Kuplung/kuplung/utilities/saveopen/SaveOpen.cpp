//
//  SaveOpen.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpen.hpp"

void SaveOpen::init() {
    this->entBinarySeq = std::make_unique<SaveOpenBinarySeq>();
    this->entBinarySeq->init();

    this->entGProtocolBufs = std::make_unique<SaveOpenGProtocolBufs>();
    this->entGProtocolBufs->init();
}

void SaveOpen::saveKuplungFile(FBEntity file, std::unique_ptr<ObjectsManager> &managerObjects, std::vector<ModelFaceBase*> meshModelFaces) {
//    this->entBinarySeq->saveKuplungFile(file, managerObjects, meshModelFaces);
    this->entGProtocolBufs->saveKuplungFile(file, managerObjects, meshModelFaces);
}

std::vector<ModelFaceData*> SaveOpen::openKuplungFile(FBEntity file, std::unique_ptr<ObjectsManager> &managerObjects) {
//    return this->entBinarySeq->openKuplungFile(file, managerObjects);
    return this->entGProtocolBufs->openKuplungFile(file, managerObjects);
}
