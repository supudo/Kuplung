//
//  SaveOpen.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpen.hpp"

void SaveOpen::init() {
    this->entBinarySeq = new SaveOpenBinarySeq();
    this->entBinarySeq->init();
}

void SaveOpen::saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces) {
    this->entBinarySeq->saveKuplungFile(file, managerObjects, meshModelFaces);
}

void SaveOpen::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    this->entBinarySeq->openKuplungFile(file, managerObjects);
}
