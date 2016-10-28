//
//  FileModelManager.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "FileModelManager.hpp"

FileModelManager::~FileModelManager() {
    this->parserOBJ2.reset();
    this->parserOBJ1.reset();
    this->parserSTL.reset();
    this->parserAssimp.reset();
}

FileModelManager::FileModelManager() {
    this->parserOBJ1 = std::make_unique<objParser1>();
    this->parserOBJ2 = std::make_unique<objParser2>();
    this->parserSTL = std::make_unique<STLParser>();
    this->parserAssimp = std::make_unique<AssimpParser>();
}

void FileModelManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
    this->parserOBJ1->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserOBJ2->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserSTL->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserAssimp->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

std::vector<MeshModel> FileModelManager::parse(FBEntity file, FileBrowser_ParserType type) {
    std::vector<MeshModel> meshModels;
    switch (type) {
        case FileBrowser_ParserType_Own1: {
            if (file.extension == ".obj" || file.extension == "obj")
                meshModels = this->parserOBJ1->parse(file);
            else if (file.extension == ".stl" || file.extension == "stl")
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
