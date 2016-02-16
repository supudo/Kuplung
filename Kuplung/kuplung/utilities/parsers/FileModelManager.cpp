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

void FileModelManager::init(std::function<void(std::string)> doLog, std::function<void(float)> doProgress) {
    this->funcLog = doLog;
    this->funcProgress = doProgress;

    this->parserOBJ = new objParser();
    this->parserOBJ->init(std::bind(&FileModelManager::logMessage, this, std::placeholders::_1), std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    //this->parserSTL = new STLParser();
    //this->parserSTL->init(std::bind(&FileModelManager::logMessage, this, std::placeholders::_1), std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

objScene FileModelManager::parse(FBEntity file) {
    //if (file.extension == ".obj")
        return this->parserOBJ->parse(file);
}

void FileModelManager::logMessage(std::string message) {
    this->funcLog("[FileModelManager] " + message);
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}

