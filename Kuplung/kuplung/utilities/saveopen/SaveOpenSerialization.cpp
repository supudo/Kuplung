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

    std::ofstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        this->storeObjectsManagerSettings(kuplungFile, managerObjects);
        this->storeGlobalLights(kuplungFile, managerObjects);

        kuplungFile.close();
    }
}

void SaveOpenSerialization::openKuplungFile(FBEntity file, ObjectsManager *managerObjects) {
    this->fileName = file.path + "/" + file.title;

    std::ifstream kuplungFile;
    kuplungFile.open(fileName.c_str(), std::ios::binary | std::ios::out | std::ios::app);

    if (kuplungFile.is_open() && !kuplungFile.bad()) {
        kuplungFile.seekg(0);

        this->readObjectsManagerSettings(kuplungFile, managerObjects);
        this->readGlobalLights(kuplungFile, managerObjects);

        kuplungFile.close();
    }
}

void SaveOpenSerialization::storeObjectsManagerSettings(std::ostream& kuplungFile, ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::readObjectsManagerSettings(std::istream& kuplungFile, ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::storeGlobalLights(std::ostream& kuplungFile, ObjectsManager *managerObjects) {
}

void SaveOpenSerialization::readGlobalLights(std::istream& kuplungFile, ObjectsManager *managerObjects) {
}

bool SaveOpenSerialization::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length())
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    else
        return false;
}
