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

void STLParser::init(std::function<void(std::string)> doLog) {
    this->doLog = doLog;
}

void STLParser::parse(FBEntity file) {
}
