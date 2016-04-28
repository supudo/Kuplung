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
    this->parserOBJ2->destroy();
    this->parserOBJ1->destroy();
    this->parserSTL->destroy();
    this->parserAssimp->destroy();
}

void FileModelManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;

    this->parserOBJ1 = new objParser1();
    this->parserOBJ1->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    this->parserOBJ2 = new objParser2();
    this->parserOBJ2->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    this->parserSTL = new STLParser();
    this->parserSTL->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));

    this->parserAssimp = new AssimpParser();
    this->parserAssimp->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

std::vector<MeshModel> FileModelManager::parse(FBEntity file, FileBrowser_ParserType type) {
    std::vector<MeshModel> meshModels;
    switch (type) {
        case FileBrowser_ParserType_Own1: {
            if (file.extension == ".obj")
                meshModels = this->parserOBJ1->parse(file);
            else if (file.extension == ".stl")
                meshModels = this->parserSTL->parse(file);
            break;
        }
        case FileBrowser_ParserType_Own2: {
            meshModels = this->parserOBJ2->parse(file);
            break;
        }
        case FileBrowser_ParserType_Assimp: {
            meshModels = this->parserAssimp->parse(file);
            break;
        }
        default:
            break;
    }
    return meshModels;
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}
