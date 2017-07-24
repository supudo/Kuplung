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

void FileModelManager::init(const std::function<void(float)>& doProgress) {
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

std::vector<MeshModel> FileModelManager::parse(FBEntity file, std::vector<std::string> settings) {
    std::vector<MeshModel> meshModels;
    if (Settings::Instance()->ModelFileParser == Importer_ParserType_Own) {
        if (Settings::Instance()->hasEnding(file.title, ".obj") || Settings::Instance()->hasEnding(file.path, ".obj"))
            meshModels = this->parserOBJ2->parse(file, settings);
        else if (Settings::Instance()->hasEnding(file.title, ".stl") || Settings::Instance()->hasEnding(file.path, ".stl"))
            meshModels = this->parserSTL->parse(file, settings);
        else if (Settings::Instance()->hasEnding(file.title, ".ply") || Settings::Instance()->hasEnding(file.path, ".ply"))
            meshModels = this->parserPLY->parse(file, settings);
    }
#ifdef DEF_KuplungSetting_UseCuda
    else if (Settings::Instance()->ModelFileParser == Importer_ParserType_Own_Cuda)
        meshModels = this->parserOBJCuda->parse(file, settings);
#endif
    else if (Settings::Instance()->ModelFileParser == Importer_ParserType_Assimp)
        meshModels = this->parserAssimp->parse(file, settings);
    return meshModels;
}

void FileModelManager::doProgress(float value) {
    this->funcProgress(value);
}
