//
//  GlTFParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "GlTFParser.hpp"
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

GlTFParser::GlTFParser() {
	this->Setting_Axis_Forward = 0;
	this->Setting_Axis_Up = 0;
}

GlTFParser::~GlTFParser() {
}

void GlTFParser::init(const std::function<void(float)>& doProgress) {
    this->doProgress = doProgress;
    this->Setting_Axis_Forward = 4;
    this->Setting_Axis_Up = 5;
    this->parserUtils = std::make_unique<ParserUtils>();
}

std::vector<MeshModel> GlTFParser::parse(const FBEntity& file, const std::vector<std::string>& settings) {
    this->models = {};

    if (settings.size() > 0 && !settings[0].empty())
        this->Setting_Axis_Forward = std::stoi(settings[0]);
    if (settings.size() > 1 && !settings[1].empty())
        this->Setting_Axis_Up = std::stoi(settings[1]);

    return this->models;
}
