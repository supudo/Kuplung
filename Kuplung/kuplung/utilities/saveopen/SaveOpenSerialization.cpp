//
//  SaveOpenSerialization.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 12/1/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "SaveOpenSerialization.hpp"

void SaveOpenSerialization::init() {
}

void SaveOpenSerialization::saveKuplungFile(FBEntity file, ObjectsManager *managerObjects, std::vector<ModelFaceBase*> meshModelFaces) {
    this->fileName = file.path + "/" + file.title;
    if (!this->hasEnding(this->fileName, ".kuplung"))
        this->fileName += ".kuplung";

    std::remove(this->fileName.c_str());

    this->storeObjectsManagerSettings(managerObjects);
    this->storeGlobalLights(managerObjects);
}

void SaveOpenSerialization::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    this->fileName = file.path + "/" + file.title;

    this->readObjectsManagerSettings(managerObjects);
    this->storeGlobalLights(managerObjects);
}

void SaveOpenSerialization::storeObjectsManagerSettings(ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::readObjectsManagerSettings(ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::storeGlobalLights(ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::readGlobalLights(ObjectsManager *managerObjects) {
}

bool SaveOpenSerialization::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
