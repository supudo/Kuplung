//
//  FileModelManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "FileModelManager.hpp"

FileModelManager::~FileModelManager() {
    this->destroy();
}

void FileModelManager::destroy() {
    this->parserOBJ->destroy();
    this->parserSTL->destroy();
    this->parserAssimp->destroy();
}

void FileModelManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;

    this->parserOBJ = new objParser();
    this->parserOBJ->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    this->parserSTL = new STLParser();
    this->parserSTL->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    this->parserAssimp = new AssimpParser();
    this->parserAssimp->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

objScene FileModelManager::parse(FBEntity file, bool useOwn) {
    if (useOwn) {
        if (file.extension == ".obj")
            return this->parserOBJ->parse(file);
        else if (file.extension == ".stl")
            return this->parserOBJ->parse(file);
        else
            return {};
    }
    else
        return this->parserAssimp->parse(file);
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}

