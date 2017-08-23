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
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "kuplung/meshes/scene/ModelFaceData.hpp"

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

	this->exportFileFolder = "";
    this->parserUtils = std::make_unique<ParserUtils>();
}

void ExporterGLTF::exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings) {
	this->exportFile = file;
	this->objSettings = settings;

	this->prepFolderLocation();

	nlohmann::json j;
	j["asset"] = { { "generator", "Kuplung" }, { "version", "1.0" } };
	j["scene"] = 0;

	j["camera"] = this->exportCamera();
	j["scenes"] = this->exportScenes(faces);
	j["nodes"] = this->exportNodes(faces);
	j["meshes"] = this->exportMeshes(faces);
	j["accessors"] = this->exportAccessors(faces);
	j["materials"] = this->exportMaterials(faces);
	j["textures"] = this->exportTextures(faces);
	j["images"] = this->exportImages(faces);
	j["samplers"] = this->exportSamplers(faces);
	j["bufferViews"] = this->exportBufferViews(faces);
	j["buffers"] = this->exportBuffers(faces, file);

	this->saveFile(j);
}

nlohmann::json ExporterGLTF::exportCamera() {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportScenes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportNodes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportMeshes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportAccessors(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportMaterials(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportTextures(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportImages(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	int c = 0;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		MeshModelMaterial mat = (*faceIterator)->meshModel.ModelMaterial;
		if (!mat.TextureAmbient.Image.empty())
			j.push_back(this->copyImage(mat.TextureAmbient.Image));
		if (!mat.TextureBump.Image.empty())
			j.push_back(this->copyImage(mat.TextureBump.Image));
		if (!mat.TextureDiffuse.Image.empty())
			j[c] = this->copyImage(mat.TextureDiffuse.Image);
		if (!mat.TextureDisplacement.Image.empty())
			j.push_back(this->copyImage(mat.TextureDisplacement.Image));
		if (!mat.TextureDissolve.Image.empty())
			j.push_back(this->copyImage(mat.TextureDissolve.Image));
		if (!mat.TextureSpecular.Image.empty())
			j.push_back(this->copyImage(mat.TextureSpecular.Image));
		if (!mat.TextureSpecularExp.Image.empty())
			j.push_back(this->copyImage(mat.TextureSpecularExp.Image));
	}
	return j;
}

nlohmann::json ExporterGLTF::copyImage(std::string imagePath) {
	nlohmann::json j;
	std::string imageFilename = imagePath.substr(imagePath.find_last_of("\\/"));
	boost::replace_all(imageFilename, "/", "");
	std::string newImagePath = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/")) + "/" + this->exportFile.title + "/" + imageFilename;
	boost::filesystem::copy_file(imagePath, newImagePath, boost::filesystem::copy_option::overwrite_if_exists);
	j["uri"] = imageFilename;
	return j;
}

nlohmann::json ExporterGLTF::exportSamplers(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportBufferViews(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	return j;
}

nlohmann::json ExporterGLTF::exportBuffers(const std::vector<ModelFaceBase*>& faces, const FBEntity& file) {
	nlohmann::json j;
	std::string bufferFile = file.title + ".bin";
	j[0]["byteLength"] = 0;
	j[0]["uri"] = bufferFile;
	return j;
}

void ExporterGLTF::prepFolderLocation() {
	std::string folder = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/"));
	std::string newFolder = folder + "/" + this->exportFile.title;
	if (!boost::filesystem::exists(newFolder)) {
		boost::filesystem::path dir(newFolder);
		if (!boost::filesystem::create_directory(dir))
			Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[ExportGLTF] Cannot create destination folder : %s!", newFolder));
	}
	this->exportFileFolder = newFolder;
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
	std::string filePath = this->exportFileFolder;
	std::string fileName = this->exportFile.title;
	if (boost::algorithm::ends_with(fileName, ".gltf"))
		fileName = fileName.substr(0, fileName.size() - 4);

	std::ofstream out(filePath + "/" + fileName + fileSuffix + ".gltf");
	out << std::setw(4) << jsonObj << std::endl;
    out.close();
}

}}}
