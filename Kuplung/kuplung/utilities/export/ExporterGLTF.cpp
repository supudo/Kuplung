//
//  ExporterGLTF.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 07/08/17.
//  Copyright © 2017 supudo.net. All rights reserved.
//

#include "ExporterGLTF.hpp"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace KuplungApp { namespace Utilities { namespace Export {

ExporterGLTF::~ExporterGLTF() {
}

void ExporterGLTF::init(const std::function<void(float)>& doProgress) {
    this->funcProgress = doProgress;
    this->addSuffix = false;
#ifdef _WIN32
    this->nlDelimiter = "\r\n";
#elif defined macintosh // OS 9
    this->nlDelimiter = "\r";
#else
    this->nlDelimiter = "\n";
#endif

    this->parserUtils = std::make_unique<ParserUtils>();
}

void ExporterGLTF::exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings) {
	this->exportFile = file;
	this->objSettings = settings;

	nlohmann::json j;
	j["asset"] = { { "generator", "Kuplung" }, { "version", "1.0" } };
	j["scene"] = 0;

	j["scenes"] = this->exportScenes(faces);
	j["nodes"] = this->exportNodes(faces);
	j["meshes"] = this->exportMeshes(faces);
	j["accessors"] = this->exportAccessors(faces);
	j["materials"] = this->exportMaterials(faces);
	j["bufferViews"] = this->exportBufferViews(faces);
	j["buffers"] = this->exportBuffers(faces);

	this->saveFile(j);
}

nlohmann::json ExporterGLTF::exportScenes(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportNodes(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportMeshes(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportAccessors(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportMaterials(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportBufferViews(const std::vector<ModelFaceBase*>& faces) {
	return {};
}

nlohmann::json ExporterGLTF::exportBuffers(const std::vector<ModelFaceBase*>& faces) {
	return {};
}


void ExporterGLTF::saveFile(const nlohmann::json& jsonObj) {
	time_t t = time(0);
	const struct tm * now = localtime(&t);

	const int year = now->tm_year + 1900;
	const int month = now->tm_mon + 1;
	const int day = now->tm_mday;
	const int hour = now->tm_hour;
	const int minute = now->tm_min;
	const int seconds = now->tm_sec;

	std::string fileSuffix = "_" +
		std::to_string(year) + std::to_string(month) + std::to_string(day) +
		std::to_string(hour) + std::to_string(minute) + std::to_string(seconds);

	if (!this->addSuffix)
		fileSuffix.clear();
	std::string filePath = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/"));
	std::string fileName = this->exportFile.title;
	if (boost::algorithm::ends_with(fileName, ".gltf"))
		fileName = fileName.substr(0, fileName.size() - 4);

    std::ofstream out(filePath + "/" + fileName + fileSuffix + ".gltf");
	out << std::setw(4) << jsonObj << std::endl;
    out.close();
}

}}}
