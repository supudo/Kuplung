//
//  AssimpParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "AssimpParser.hpp"

AssimpParser::~AssimpParser() {
    this->destroy();
}

void AssimpParser::destroy() {
}

void AssimpParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

objScene AssimpParser::parse(FBEntity file) {
    Settings::Instance()->funcDoLog("[Assimp] Start..." + file.title);
    const aiScene* scene = this->parser.ReadFile(file.path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Settings::Instance()->funcDoLog("[Assimp] Parse error : " + std::string(this->parser.GetErrorString()));
        return {};
    }
    Settings::Instance()->funcDoLog("[Assimp] End - " + std::to_string(scene->mNumMeshes));
    return {};
}
