//
//  STLParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "STLParser.hpp"

STLParser::~STLParser() {
}

void STLParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

std::vector<MeshModel> STLParser::parse(FBEntity file, std::vector<std::string> settings) {
    return {};
}
