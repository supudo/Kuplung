//
//  GLTFParser.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#include "GLTFParser.hpp"
#include "kuplung/utilities/cpp-base64/base64.h"
#include "kuplung/utilities/helpers/Strings.h"
#include <sstream>
#include <nlohmann/json.hpp>

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
	nlohmann::json j;
	jsonStream >> j;
	
	std::string infileBuffer = "data:application/octet-stream;base64,";
	std::vector<std::string> dataBuffers;
	for (auto const& buffer : j["buffers"]) {
		std::string dataURI = buffer.at("uri").get<std::string>();
    if (dataURI.starts_with(infileBuffer)) {
      if (dataURI.find(infileBuffer) != std::string::npos)
        dataURI = dataURI.replace(dataURI.find(infileBuffer), infileBuffer.length(), "");
			dataBuffers.push_back(base64_decode(dataURI));
		}
		else {
			// TODO: process external bin file
		}
	}
	
	nlohmann::json jScenes = j["scenes"];
	for (auto const& js : jScenes) {
		nlohmann::json sceneNodes = js["nodes"];
		for (int const jsNodeIndex : sceneNodes) {
			nlohmann::json jNode = j["nodes"][jsNodeIndex];
			
			MeshModel mm = {};
			mm.ModelTitle = jNode.at("name").get<std::string>();
			mm.ID = jsNodeIndex;
			mm.File = file;

			int meshIndex = jNode.at("mesh").get<int>();
			nlohmann::json jnm = j["meshes"][meshIndex];
			auto jprimitives = jnm["primitives"];
			for (auto const& jp : jprimitives) {
				int indexIndices = jp.at("indices").get<int>();
				int indexMaterial = -1;
				if (jp.count("material") > 0)
					indexMaterial = jp.at("material").get<int>();
				// TODO: "mode"
				// 0 - points
				// 1 - line
				// 2 - line_loop
				// 4 - triangles
				// 5 - triangles_strip
				// 6 - triangles_fan
				int indexVertices = jp.at("attributes").at("POSITION").get<int>();
				int indexNormals = jp.at("attributes").at("NORMAL").get<int>();
				int indexTexture = -1;
				if (jp.at("attributes").count("TEXCOORD_0") > 0)
					indexTexture = jp.at("attributes").at("TEXCOORD_0").get<int>();

				auto accssIndices = j["accessors"][indexIndices];
				int bvIndexIndices = accssIndices.at("bufferView");
				auto bvIndices = j["bufferViews"][bvIndexIndices];
				int bIndexIndices = bvIndices.at("buffer").get<int>();
				int bByteLengthIndices = bvIndices.at("byteLength").get<int>();
				int bByteOffsetIndices = bvIndices.at("byteOffset").get<int>();
				const char* cIndices = dataBuffers[bIndexIndices].c_str();
				for (int i = 0; i < bByteLengthIndices; i++) {
				}
			}

			this->models.push_back(mm);
		}
	}

    return this->models;
}
