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
#ifdef DEF_KuplungSetting_UseCuda
    this->parserOBJCuda.reset();
#endif
    this->parserSTL.reset();
    this->parserPLY.reset();
    this->parserAssimp.reset();
}

FileModelManager::FileModelManager() {
    this->parserOBJ1 = std::make_unique<objParser1>();
    this->parserOBJ2 = std::make_unique<objParser2>();
#ifdef DEF_KuplungSetting_UseCuda
    this->parserOBJCuda = std::make_unique<objParserCuda>();
#endif
    this->parserSTL = std::make_unique<STLParser>();
    this->parserPLY = std::make_unique<PLYParser>();
    this->parserAssimp = std::make_unique<AssimpParser>();
}

void FileModelManager::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
    this->parserOBJ1->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserOBJ2->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
#ifdef DEF_KuplungSetting_UseCuda
    this->parserOBJCuda->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
#endif
    this->parserSTL->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserPLY->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
    this->parserAssimp->init(std::bind(&FileModelManager::doProgress, this, std::placeholders::_1));
}

std::vector<MeshModel> FileModelManager::parse(FBEntity file, FileBrowser_ParserType type, std::vector<std::string> settings) {
    std::vector<MeshModel> meshModels;
    assert(type == FileBrowser_ParserType_Own1 ||
           type == FileBrowser_ParserType_Own2 ||
#ifdef DEF_KuplungSetting_UseCuda
           type == FileBrowser_ParserType_Cuda ||
#endif
           type == FileBrowser_ParserType_STL ||
           type == FileBrowser_ParserType_PLY ||
           type == FileBrowser_ParserType_Assimp);
    switch (type) {
        case FileBrowser_ParserType_Own1: {
            meshModels = this->parserOBJ1->parse(file, settings);
            break;
        }
        case FileBrowser_ParserType_Own2: {
            meshModels = this->parserOBJ2->parse(file, settings);
            break;
        }
        case FileBrowser_ParserType_Assimp: {
            meshModels = this->parserAssimp->parse(file, settings);
            break;
        }
#ifdef DEF_KuplungSetting_UseCuda
        case FileBrowser_ParserType_Cuda: {
            meshModels = this->parserOBJCuda->parse(file, settings);
            break;
        }
#endif
        case FileBrowser_ParserType_STL: {
            meshModels = this->parserSTL->parse(file, settings);
            break;
        }
        case FileBrowser_ParserType_PLY: {
            meshModels = this->parserPLY->parse(file, settings);
            break;
        }
    }
    return meshModels;
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}
