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
#include "kuplung/utilities/cpp-base64/base64.h"
#include <glm/gtx/string_cast.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/endian/conversion.hpp>

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
	this->gltfGenerator = "Kuplung";
	this->defaultSceneName = "KuplungScene";
	this->defaultMaterialName = "KuplungMaterial";
	this->gltfVersion = "2.0";
}

void ExporterGLTF::exportToFile(const FBEntity& file, const std::vector<ModelFaceBase*>& faces, const std::vector<std::string>& settings, std::unique_ptr<ObjectsManager> &managerObjects) {
    this->exportFile = file;
    this->objSettings = settings;

    this->prepFolderLocation();

    nlohmann::json j;
    j["asset"] = { { "generator", "Kuplung" }, { this->gltfGenerator, this->gltfVersion } };
    j["scene"] = this->defaultSceneName;
	j["scenes"] = this->exportScenes(faces);
    j["cameras"] = this->exportCameras(managerObjects);
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

nlohmann::json ExporterGLTF::exportCameras(std::unique_ptr<ObjectsManager> &managerObjects) {
    nlohmann::json j;
    nlohmann::json viewportCamera;
    viewportCamera["type"] = "perspective";
    viewportCamera["perspective"] = {
        { "aspectRatio", managerObjects->Setting_RatioWidth / managerObjects->Setting_RatioHeight },
        { "yfov", managerObjects->Setting_FOV },
        { "zfar", managerObjects->Setting_PlaneClose },
        { "znear", managerObjects->Setting_PlaneFar }
    };
    j.push_back(viewportCamera);
    return j;
}

nlohmann::json ExporterGLTF::exportScenes(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		j[this->defaultSceneName]["nodes"][faceIterator - faces.begin()] = model.ModelTitle;
	}
    return j;
}

nlohmann::json ExporterGLTF::exportNodes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		j[model.ModelTitle][faceIterator - faces.begin()] = model.ModelTitle + "_mesh";
	}
	return j;
}

nlohmann::json ExporterGLTF::exportMeshes(const std::vector<ModelFaceBase*>& faces) {
	nlohmann::json j;
	std::vector<ModelFaceBase*>::const_iterator faceIterator;
	for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
		const ModelFaceBase& face = **faceIterator;
		MeshModel model = face.meshModel;
		nlohmann::json jMeshPrimitives;
		if (model.texture_coordinates.size() > 0) {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", this->defaultMaterialName },
				{ "indices", model.ModelTitle + "_mesh_accesor_indices" },
				{ "attributes", {
						{ "POSITION", model.ModelTitle + "_mesh_accesor_vertices" },
						{ "NORMAL", model.ModelTitle + "_mesh_accesor_normals" },
						{ "TEXCOORD_0", model.ModelTitle + "_mesh_accesor_uvs" }
					}
				}
			};
		}
		else {
			jMeshPrimitives[faceIterator - faces.begin()] = {
				{ "mode", 4 },
				{ "material", this->defaultMaterialName },
				{ "indices", model.ModelTitle + "_mesh_accesor_indices" },
				{ "attributes", {
						{ "POSITION", model.ModelTitle + "_mesh_accesor_vertices" },
						{ "NORMAL", model.ModelTitle + "_mesh_accesor_normals" }
					}
				}
			};
		}
		j[model.ModelTitle + "_mesh"] = {
			{ "name", model.ModelTitle + "_mesh" },
			{ "primitives", jMeshPrimitives }
		};
	}
	return j;
}

nlohmann::json ExporterGLTF::exportAccessors(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
    return j;
}

nlohmann::json ExporterGLTF::exportMaterials(const std::vector<ModelFaceBase*>& faces) {
    nlohmann::json j;
	j[this->defaultMaterialName]["value"]["ambient"] = { 0.0f, 0.0f, 0.0f, 1.0f };
	j[this->defaultMaterialName]["value"]["diffuse"] = { 0.0f, 0.0f, 0.0f, 1.0f };
	j[this->defaultMaterialName]["value"]["specular"] = { 0.0f, 0.0f, 0.0f, 1.0f };
	j[this->defaultMaterialName]["value"]["emission"] = { 0.0f, 0.0f, 0.0f, 1.0f };
	j[this->defaultMaterialName]["value"]["shininess"] = 0.0f;
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
    std::vector<ModelFaceBase*>::const_iterator faceIterator;
	nlohmann::json jBuffer, jBufferViews;
	int totalBufferLength = 0;
	std::string bufferData;
    for (faceIterator = faces.begin(); faceIterator != faces.end(); ++faceIterator) {
        const ModelFaceBase& face = **faceIterator;
        MeshModel model = face.meshModel;
		int base64_size_indices = model.indices.size() * sizeof(float);
        int base64_size_vertices = model.vertices.size() * sizeof(float);
		int base64_size_normals = model.normals.size() * sizeof(float);
		int base64_size_textureCoordinates = model.texture_coordinates.size() * sizeof(float);
		std::string base64_indices = base64_encode(reinterpret_cast<unsigned char const *>(model.indices.data()), base64_size_indices);
        std::string base64_vertices = base64_encode(reinterpret_cast<unsigned char const *>(model.vertices.data()), base64_size_vertices);
		std::string base64_normals = base64_encode(reinterpret_cast<unsigned char const *>(model.normals.data()), base64_size_normals);
		std::string base64_textureCoordinates = base64_encode(reinterpret_cast<unsigned char const *>(model.texture_coordinates.data()), base64_size_textureCoordinates);
		totalBufferLength += base64_size_indices + base64_size_vertices + base64_size_normals + base64_size_textureCoordinates;
		bufferData += base64_indices + base64_vertices + base64_normals + base64_textureCoordinates;
		jBufferViews[model.ModelTitle + "_view_indices"] = {
			{ "buffer", "" }
		};
    }
	jBuffer["type"] = "arraybuffer";
	jBuffer["byteLength"] = totalBufferLength;
	//jBuffer["uri"] = "data:application/octet-stream;base64," + bufferData;
	if (!this->saveBufferFile(bufferData))
		Settings::Instance()->funcDoLog("[Kuplung] Error occured while writing the BIN glTF file!");
	jBuffer["uri"] = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";
	j[this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin"] = jBuffer;
    return j;
}

void ExporterGLTF::prepFolderLocation() {
    std::string folder = this->exportFile.path.substr(0, this->exportFile.path.find_last_of("\\/"));
    std::string newFolder = folder + "/" + this->exportFile.title;
    if (!boost::filesystem::exists(newFolder)) {
        boost::filesystem::path dir(newFolder);
        if (!boost::filesystem::create_directory(dir))
            Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[ExportGLTF] Cannot create destination folder : %s!", newFolder.c_str()));
    }
    this->exportFileFolder = newFolder;
}

bool ExporterGLTF::saveFile(const nlohmann::json& jsonObj) {
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

	return true;
}

bool ExporterGLTF::saveBufferFile(std::string buffer) {
	std::string file = this->exportFileFolder + "/" + this->exportFile.title + ".gltf.bin";

	auto f = fopen(file.c_str(), "wt");
	if (!f) {
		Settings::Instance()->funcDoLog("[Kuplung] Could not write glTF binary file!");
		return false;
	}

	uint32_t buffer_length = buffer.size();
	if (buffer_length % 4)
		buffer_length += 4 - buffer_length % 4;

	// 12-byte header
	// - magic
	uint32_t magic = 0x46546C67;
	if (!std::fwrite(&magic, 1, 1, f))
		return false;

	// - version
	uint32_t version = 2;
	if (!std::fwrite(&version, 1, 1, f))
		return false;

	// - length
	uint32_t length = 12 + 8 + buffer_length;
	if (!std::fwrite(&length, 1, 1, f))
		return false;

	// binary buffer chunk
	// - chunk length
	if (!std::fwrite(&buffer_length, 1, 1, f))
		return false;

	// - chunk type
	uint32_t chunkType = 0x004E4942;
	if (!std::fwrite(&chunkType, 1, 1, f))
		return false;

	// - chunkData
	if (!std::fwrite((char*)&buffer, 1, (int)buffer.size(), f))
		return false;

	// padding
	char pad = 0;
	for (auto i = 0; i < buffer_length - buffer.size(); i++) {
		if (!fwrite(&pad, 1, 1, f))
			return false;
	}

	return true;
}

}}}
