//
//  PLYParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "PLYParser.hpp"

PLYParser::~PLYParser() {
}

void PLYParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

std::vector<MeshModel> PLYParser::parse(const FBEntity&, const std::vector<std::string>&) {
    return {};
}
