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

objScene FileModelManager::parse(FBEntity file, FileBrowser_ParserType type) {
    switch (type) {
        case FileBrowser_ParserType_Own: {
            if (file.extension == ".obj")
                return this->parserOBJ->parse(file);
            else if (file.extension == ".stl")
                return this->parserOBJ->parse(file);
            break;
        }
        case FileBrowser_ParserType_Assimp: {
            return this->parserAssimp->parse(file);
            break;
        }
        default:
            return {};
            break;
    }
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}

