//
//  GLTFParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "GLTFParser.hpp"
#include <sstream>
#include "kuplung/utilities/json/json.hpp"

GLTFParser::GLTFParser() {
}

GLTFParser::~GLTFParser() {
}

void GLTFParser::init(const std::function<void(float)>& doProgress) {
    this->doProgress = doProgress;
}

std::vector<MeshModel> GLTFParser::parse(const FBEntity& file, const std::vector<std::string>& settings) {
    this->models = {};

	std::ifstream jsonStream(file.path);
	if (!jsonStream.is_open()) {
		Settings::Instance()->funcDoLog("[GLTFParser] Cannot open .gltf file" + file.path + "!");
		jsonStream.close();
		return {};
	}
	nlohmann::json jsonObj;
	jsonStream >> jsonObj;
	
	//printf("jsonObj generator = %s\n", jsonObj.at("asset").at("generator").get<std::string>().c_str());

	/*
	
	mesh mode :
		0 - points
		1 - line
		2 - line_loop
		4 - triangles
		5 - triangles_strip
		6 - triangles_fan
	
	*/

    return this->models;
}
