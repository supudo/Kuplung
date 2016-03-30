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
}

void FileModelManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;

    this->parserOBJ = new objParser();
    this->parserOBJ->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    //this->parserSTL = new STLParser();
    //this->parserSTL->init(sstd::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

objScene FileModelManager::parse(FBEntity file) {
    //if (file.extension == ".obj")
        return this->parserOBJ->parse(file);
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}

