//
//  STLParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 1/2/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "STLParser.hpp"

STLParser::~STLParser() {
    this->destroy();
}

void STLParser::destroy() {
}

void STLParser::init(std::function<void(float)> doProgress) {
    this->funcProgress = doProgress;
}

objScene STLParser::parse(FBEntity file) {
    return {};
}
